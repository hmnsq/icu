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

#ಯದಿ !UCONFIG_NO_CONVERSION

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


#ಯದಿ 0
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಬಾಹ್ಯ ಶೂನ್ಯ UCNV_DEBUG_LOG(ವರ್ಣ *what, ವರ್ಣ *who, ಶೂನ್ಯ *p, ಪೂರ್ಣಾಙ್ಕ l);
#define UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#ಅನ್ಯಥಾ
# define UCNV_DEBUG_LOG(x,y,z)
#endif

ಸ್ಥಿರ ಧ್ರುವ UConverterSharedData * ಧ್ರುವ
converterData[UCNV_ಸಂಖ್ಯೆ_OF_SUPPORTED_CONVERTER_TYPES]={
    nullptr, nullptr,

#ಯದಿ UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#ಅನ್ಯಥಾ
    &_MBCSData,
#endif

    &_Latin1Data,
    &_UTF8Data, &_UTF16BEData, &_UTF16LEData,
#ಯದಿ UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr,
#ಅನ್ಯಥಾ
    &_UTF32BEData, &_UTF32LEData,
#endif
    nullptr,

#ಯದಿ UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#ಅನ್ಯಥಾ
    &_ISO2022Data,
#endif

#ಯದಿ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr,
#ಅನ್ಯಥಾ
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#endif

#ಯದಿ UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#ಅನ್ಯಥಾ
    &_SCSUData,
#endif


#ಯದಿ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#ಅನ್ಯಥಾ
    &_ISCIIData,
#endif

    &_ASCIIData,
#ಯದಿ UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, &_UTF16Data, nullptr, nullptr, nullptr,
#ಅನ್ಯಥಾ
    &_UTF7Data, &_Bocu1Data, &_UTF16Data, &_UTF32Data, &_CESU8Data, &_IMAPData,
#endif

#ಯದಿ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#ಅನ್ಯಥಾ
    &_CompoundTextData
#endif
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
ಸ್ಥಿರ ಸಂರಚನಾ {
  ಧ್ರುವ ವರ್ಣ *name;
  ಧ್ರುವ UConverterType type;
} ಧ್ರುವ cnvNameType[] = {
#ಯದಿ !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#endif
#ಯದಿ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#endif
#ಯದಿ !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#endif
#ಯದಿ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#endif
#ಯದಿ !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#endif
  { "iso88591", UCNV_LATIN_1 },
#ಯದಿ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#endif
#ಯದಿ !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#endif
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#ಯದಿ U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#ಅನ್ಯಥಾ
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#endif
#ಯದಿ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#ಯದಿ U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#ಅನ್ಯಥಾ
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#endif
#endif
#ಯದಿ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#endif
  { "utf8", UCNV_UTF8 },
#ಯದಿ !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_TEXT}
#endif
};


/*initializes some global variables */
ಸ್ಥಿರ UHashtable *SHARED_DATA_HASHTABLE = nullptr;
ಸ್ಥಿರ icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ **gAvailableConverters = nullptr;
ಸ್ಥಿರ uint16_t g_Available_Converter_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ icu::UInitOnce gAvailableConvertersInitOnce {};

#ಯದಿ !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
ಸ್ಥಿರ ವರ್ಣ gDefaultConverterNameBuffer[UCNV_MAX_CONVERTER_NAME_LENGTH + 1]; /* +1 for nullptr */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *gDefaultConverterName = nullptr;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
ಸ್ಥಿರ ಧ್ರುವ UConverterSharedData *gDefaultAlgorithmicSharedData = nullptr;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
ಸ್ಥಿರ UBool gDefaultConverterContainsOption;

#endif  /* !U_CHARSET_IS_UTF8 */

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ DATA_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_flushAvailableConverterCache() {
    g_Available_Converter_ಎಣಿಕೆ = 0;
    ಯದಿ (gAvailableConverters) {
        uprv_free(const_cast<ವರ್ಣ**>(gAvailableConverters));
        gAvailableConverters = nullptr;
    }
    gAvailableConvertersInitOnce.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
ಸ್ಥಿರ UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    ಯದಿ (SHARED_DATA_HASHTABLE != nullptr && uhash_ಎಣಿಕೆ(SHARED_DATA_HASHTABLE) == 0) {
        uhash_close(SHARED_DATA_HASHTABLE);
        SHARED_DATA_HASHTABLE = nullptr;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flushAvailableConverterCache();

#ಯದಿ !U_CHARSET_IS_UTF8
    gDefaultConverterName = nullptr;
    gDefaultConverterNameBuffer[0] = 0;
    gDefaultConverterContainsOption = ಅಸತ್ಯ;
    gDefaultAlgorithmicSharedData = nullptr;
#endif

    ಪ್ರತ್ಯಾಗಮ (SHARED_DATA_HASHTABLE == nullptr);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_registerCleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

ಸ್ಥಿರ UBool U_CALLCONV
isCnvAcceptable(ಶೂನ್ಯ * /*context*/,
                ಧ್ರುವ ವರ್ಣ * /*type*/, ಧ್ರುವ ವರ್ಣ * /*name*/,
                ಧ್ರುವ UDataInfo *pInfo) {
    ಪ್ರತ್ಯಾಗಮ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->dataFormat[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x76 &&
        pInfo->dataFormat[3]==0x74 &&
        pInfo->formatVersion[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
ಸ್ಥಿರ UConverterSharedData*
ucnv_data_unFlattenClone(UConverterLoadArgs *pArgs, UDataMemory *pData, UErrorCode *status)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ಧ್ರುವ uint8_t* raw = static_cast<ಧ್ರುವ uint8_t*>(udata_getMemory(pData));
    ಧ್ರುವ UConverterStaticData* source = reinterpret_cast<ಧ್ರುವ UConverterStaticData*>(raw);
    UConverterSharedData *data;
    UConverterType type = static_cast<UConverterType>(source->conversionType);

    ಯದಿ(U_FAILURE(*status))
        ಪ್ರತ್ಯಾಗಮ nullptr;

    ಯದಿ (static_cast<uint16_t>(type) >= UCNV_ಸಂಖ್ಯೆ_OF_SUPPORTED_CONVERTER_TYPES ||
        converterData[type] == nullptr ||
        !converterData[type]->isReferenceCounted ||
        converterData[type]->referenceCounter != 1 ||
        source->structSize != ಪರಿಮಾಣ(UConverterStaticData))
    {
        *status = U_INVALID_TABLE_FORMAT;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    data = static_cast<UConverterSharedData*>(uprv_malloc(ಪರಿಮಾಣ(UConverterSharedData)));
    ಯದಿ(data == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(data, converterData[type], ಪರಿಮಾಣ(UConverterSharedData));

    data->staticData = source;

    data->sharedDataCached = ಅಸತ್ಯ;

    /* fill in fields from the loaded data */
    data->dataMemory = (ಶೂನ್ಯ*)pData; /* for future use */

    ಯದಿ(data->impl->load != nullptr) {
        data->impl->load(data, pArgs, raw + source->structSize, status);
        ಯದಿ(U_FAILURE(*status)) {
            uprv_free(data);
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
    }
    ಪ್ರತ್ಯಾಗಮ data;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
ಸ್ಥಿರ UConverterSharedData *createConverterFromFile(UConverterLoadArgs *pArgs, UErrorCode * err)
{
    UDataMemory *data;
    UConverterSharedData *sharedData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_LOAD);

    ಯದಿ (U_FAILURE (*err)) {
        UTRACE_EXIT_STATUS(*err);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", pArgs->name, pArgs->pkg);

    data = udata_openChoice(pArgs->pkg, DATA_TYPE, pArgs->name, isCnvAcceptable, nullptr, err);
    ಯದಿ(U_FAILURE(*err))
    {
        UTRACE_EXIT_STATUS(*err);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    sharedData = ucnv_data_unFlattenClone(pArgs, data, err);
    ಯದಿ(U_FAILURE(*err))
    {
        udata_close(data);
        UTRACE_EXIT_STATUS(*err);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_STATUS(sharedData, *err);
    ಪ್ರತ್ಯಾಗಮ sharedData;
}

/*returns a converter type from a string
 */
ಸ್ಥಿರ ಧ್ರುವ UConverterSharedData *
getAlgorithmicTypeFromName(ಧ್ರುವ ವರ್ಣ *realName)
{
    uint32_t mid, start, limit;
    uint32_t lastMid;
    ಪೂರ್ಣಾಙ್ಕ result;
    ವರ್ಣ strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    start = 0;
    limit = UPRV_LENGTHOF(cnvNameType);
    mid = limit;
    lastMid = UINT32_MAX;

    ಪ್ರತಿ (;;) {
        mid = (start + limit) / 2;
        ಯದಿ (lastMid == mid) {   /* Have we moved? */
            ಭಙ್ಗ;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        result = uprv_strcmp(strippedName, cnvNameType[mid].name);

        ಯದಿ (result < 0) {
            limit = mid;
        } ಅನ್ಯಥಾ ಯದಿ (result > 0) {
            start = mid;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ converterData[cnvNameType[mid].type];
        }
    }

    ಪ್ರತ್ಯಾಗಮ nullptr;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#define UCNV_CACHE_LOAD_FACTOR 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_shareConverterData(UConverterSharedData * data)
{
    UErrorCode err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    ಯದಿ (SHARED_DATA_HASHTABLE == nullptr)
    {
        SHARED_DATA_HASHTABLE = uhash_openSize(uhash_hashChars, uhash_compareChars, nullptr,
                            ucnv_io_ಎಣಿಕೆ_Known_Converters(&err)*UCNV_CACHE_LOAD_FACTOR,
                            &err);
        ucnv_enableCleanup();

        ಯದಿ (U_FAILURE(err))
            ಪ್ರತ್ಯಾಗಮ;
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
    data->sharedDataCached = ಸತ್ಯ;

    uhash_put(SHARED_DATA_HASHTABLE,
            (ಶೂನ್ಯ*) data->staticData->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            data,
            &err);
    UCNV_DEBUG_LOG("put", data->staticData->name,data);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
ಸ್ಥಿರ UConverterSharedData *
ucnv_getSharedConverterData(ಧ್ರುವ ವರ್ಣ *name)
{
    /*special case when no Table has yet been created we return nullptr */
    ಯದಿ (SHARED_DATA_HASHTABLE == nullptr)
    {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಅನ್ಯಥಾ
    {
        UConverterSharedData *rc;

        rc = static_cast<UConverterSharedData*>(uhash_get(SHARED_DATA_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        ಪ್ರತ್ಯಾಗಮ rc;
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
ucnv_deleteSharedConverterData(UConverterSharedData * deadSharedData)
{
    UTRACE_ENTRY_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", deadSharedData->staticData->name, deadSharedData);

    ಯದಿ (deadSharedData->referenceCounter > 0) {
        UTRACE_EXIT_ಮೌಲ್ಯ((int32_t)ಅಸತ್ಯ);
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    ಯದಿ (deadSharedData->impl->unload != nullptr) {
        deadSharedData->impl->unload(deadSharedData);
    }

    ಯದಿ(deadSharedData->dataMemory != nullptr)
    {
        UDataMemory *data = (UDataMemory*)deadSharedData->dataMemory;
        udata_close(data);
    }

    uprv_free(deadSharedData);

    UTRACE_EXIT_ಮೌಲ್ಯ((int32_t)ಸತ್ಯ);
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverterSharedData *
ucnv_load(UConverterLoadArgs *pArgs, UErrorCode *err) {
    UConverterSharedData *mySharedConverterData;

    ಯದಿ(err == nullptr || U_FAILURE(*err)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ(pArgs->pkg != nullptr && *pArgs->pkg != 0) {
        /* application-provided converters are not currently cached */
        ಪ್ರತ್ಯಾಗಮ createConverterFromFile(pArgs, err);
    }

    mySharedConverterData = ucnv_getSharedConverterData(pArgs->name);
    ಯದಿ (mySharedConverterData == nullptr)
    {
        /*Not cached, we need to stream it in from file */
        mySharedConverterData = createConverterFromFile(pArgs, err);
        ಯದಿ (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಅನ್ಯಥಾ ಯದಿ (!pArgs->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_shareConverterData(mySharedConverterData);
        }
    }
    ಅನ್ಯಥಾ
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        mySharedConverterData->referenceCounter++;
    }

    ಪ್ರತ್ಯಾಗಮ mySharedConverterData;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI ಶೂನ್ಯ
ucnv_unload(UConverterSharedData *sharedData) {
    ಯದಿ(sharedData != nullptr) {
        ಯದಿ (sharedData->referenceCounter > 0) {
            sharedData->referenceCounter--;
        }

        ಯದಿ((sharedData->referenceCounter <= 0)&&(sharedData->sharedDataCached == ಅಸತ್ಯ)) {
            ucnv_deleteSharedConverterData(sharedData);
        }
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_unloadSharedDataIfReady(UConverterSharedData *sharedData)
{
    ಯದಿ(sharedData != nullptr && sharedData->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        ucnv_unload(sharedData);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_increment_Ref_ಎಣಿಕೆ(UConverterSharedData *sharedData)
{
    ಯದಿ(sharedData != nullptr && sharedData->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        sharedData->referenceCounter++;
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
parseConverterOptions(ಧ್ರುವ ವರ್ಣ *inName,
                      UConverterNamePieces *pPieces,
                      UConverterLoadArgs *pArgs,
                      UErrorCode *err)
{
    ವರ್ಣ *cnvName = pPieces->cnvName;
    ವರ್ಣ c;
    int32_t len = 0;

    pArgs->name=inName;
    pArgs->locale=pPieces->locale;
    pArgs->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    ಯಾವತ್((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        ಯದಿ (++len>=UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *err = U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            ಪ್ರತ್ಯಾಗಮ;
        }
        *cnvName++=c;
        inName++;
    }
    *cnvName=0;
    pArgs->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    ಯಾವತ್((c=*inName)!=0) {
        ಯದಿ(c==UCNV_OPTION_SEP_CHAR) {
            ++inName;
        }

        /* inName is behind an option separator */
        ಯದಿ(uprv_strncmp(inName, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            ವರ್ಣ *dest=pPieces->locale;

            /* copy the locale option value */
            inName+=7;
            len=0;
            ಯಾವತ್((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++inName;

                ಯದಿ(++len>=ULOC_FULLNAME_CAPACITY) {
                    *err=U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    ಪ್ರತ್ಯಾಗಮ;
                }

                *dest++=c;
            }
            *dest=0;
        } ಅನ್ಯಥಾ ಯದಿ(uprv_strncmp(inName, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            inName+=8;
            c=*inName;
            ಯದಿ(c==0) {
                pArgs->options=(pPieces->options&=~UCNV_OPTION_VERSION);
                ಪ್ರತ್ಯಾಗಮ;
            } ಅನ್ಯಥಾ ಯದಿ (static_cast<uint8_t>(c - '0') < 10) {
                pArgs->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_VERSION) | static_cast<uint32_t>(c - '0');
                ++inName;
            }
        } ಅನ್ಯಥಾ ಯದಿ(uprv_strncmp(inName, "swaplfnl", 8)==0) {
            inName+=8;
            pArgs->options=(pPieces->options|=UCNV_OPTION_SWAP_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } ಅನ್ಯಥಾ {
            /* ignore any other options until we define some */
            ಯಾವತ್(((c = *inName++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            ಯದಿ(c==0) {
                ಪ್ರತ್ಯಾಗಮ;
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
U_CFUNC UConverterSharedData *
ucnv_loadSharedData(ಧ್ರುವ ವರ್ಣ *converterName,
                    UConverterNamePieces *pPieces,
                    UConverterLoadArgs *pArgs,
                    UErrorCode * err) {
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverterLoadArgs ರಾಶಿ_Args;
    UConverterSharedData *mySharedConverterData = nullptr;
    UErrorCode internalErrorCode = U_ZERO_ERROR;
    UBool mayContainOption = ಸತ್ಯ;
    UBool checkForAlgorithmic = ಸತ್ಯ;

    ಯದಿ (U_FAILURE (*err)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ(pPieces == nullptr) {
        ಯದಿ(pArgs != nullptr) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_INTERNAL_PROGRAM_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        pPieces = &ರಾಶಿ_Pieces;
    }
    ಯದಿ(pArgs == nullptr) {
        uprv_memset(&ರಾಶಿ_Args, 0, ಪರಿಮಾಣ(ರಾಶಿ_Args));
        ರಾಶಿ_Args.size = (int32_t)ಪರಿಮಾಣ(ರಾಶಿ_Args);
        pArgs = &ರಾಶಿ_Args;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    pArgs->name = converterName;
    pArgs->locale = pPieces->locale;
    pArgs->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    ಯದಿ (converterName == nullptr) {
#ಯದಿ U_CHARSET_IS_UTF8
        pArgs->name = "UTF-8";
        ಪ್ರತ್ಯಾಗಮ (UConverterSharedData *)converterData[UCNV_UTF8];
#ಅನ್ಯಥಾ
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        pArgs->name = ucnv_getDefaultName();
        ಯದಿ (pArgs->name == nullptr) {
            *err = U_MISSING_RESOURCE_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        mySharedConverterData = (UConverterSharedData *)gDefaultAlgorithmicSharedData;
        checkForAlgorithmic = ಅಸತ್ಯ;
        mayContainOption = gDefaultConverterContainsOption;
        /* the default converter name is already canonical */
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        pArgs->name = "UTF-8";
        ಪ್ರತ್ಯಾಗಮ (UConverterSharedData *)converterData[UCNV_UTF8];
    }
    ಅನ್ಯಥಾ {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, pArgs, err);
        ಯದಿ (U_FAILURE(*err)) {
            /* Very bad name used. */
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }

        /* get the canonical converter name */
        pArgs->name = ucnv_io_getConverterName(pArgs->name, &mayContainOption, &internalErrorCode);
        ಯದಿ (U_FAILURE(internalErrorCode) || pArgs->name == nullptr) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            pArgs->name = pPieces->cnvName;
        } ಅನ್ಯಥಾ ಯದಿ (internalErrorCode == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    ಯದಿ(mayContainOption && pArgs->name != pPieces->cnvName) {
        parseConverterOptions(pArgs->name, pPieces, pArgs, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    ಯದಿ (checkForAlgorithmic) {
        mySharedConverterData = (UConverterSharedData *)getAlgorithmicTypeFromName(pArgs->name);
    }
    ಯದಿ (mySharedConverterData == nullptr)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        pArgs->nestedLoads=1;
        pArgs->pkg=nullptr;

        umtx_lock(&cnvCacheMutex);
        mySharedConverterData = ucnv_load(pArgs, err);
        umtx_unlock(&cnvCacheMutex);
        ಯದಿ (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
    }

    ಪ್ರತ್ಯಾಗಮ mySharedConverterData;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ಧ್ರುವ ವರ್ಣ* converterName,
                     UErrorCode* err)
{
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverterLoadArgs ರಾಶಿ_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    ಯದಿ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        mySharedConverterData = ucnv_loadSharedData(converterName, &ರಾಶಿ_Pieces, &ರಾಶಿ_Args, err);

        myUConverter = ucnv_createConverterFromSharedData(
            myUConverter, mySharedConverterData,
            &ರಾಶಿ_Args,
            err);

        ಯದಿ(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
            ಪ್ರತ್ಯಾಗಮ myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_STATUS(*err);
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CFUNC UBool
ucnv_canCreateConverter(ಧ್ರುವ ವರ್ಣ *converterName, UErrorCode *err) {
    UConverter myUConverter;
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverterLoadArgs ರಾಶಿ_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    ಯದಿ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ರಾಶಿ_Args.onlyTestIsLoadable=ಸತ್ಯ;
        mySharedConverterData = ucnv_loadSharedData(converterName, &ರಾಶಿ_Pieces, &ರಾಶಿ_Args, err);
        ucnv_createConverterFromSharedData(
            &myUConverter, mySharedConverterData,
            &ರಾಶಿ_Args,
            err);
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
    }

    UTRACE_EXIT_STATUS(*err);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ಧ್ರುವ ವರ್ಣ* locale,
                                uint32_t options,
                                UErrorCode* err) {
    UConverter *cnv;
    ಧ್ರುವ UConverterSharedData *sharedData;
    UConverterLoadArgs ರಾಶಿ_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    ಯದಿ(type<0 || UCNV_ಸಂಖ್ಯೆ_OF_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    sharedData = converterData[type];
    ಯದಿ(sharedData == nullptr || sharedData->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ರಾಶಿ_Args.name = "";
    ರಾಶಿ_Args.options = options;
    ರಾಶಿ_Args.locale=locale;
    cnv = ucnv_createConverterFromSharedData(
            myUConverter, const_cast<UConverterSharedData*>(sharedData),
            &ರಾಶಿ_Args, err);

    UTRACE_EXIT_PTR_STATUS(cnv, *err);
    ಪ್ರತ್ಯಾಗಮ cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ಧ್ರುವ ವರ್ಣ *packageName, ಧ್ರುವ ವರ್ಣ *converterName, UErrorCode * err)
{
    UConverter *myUConverter;
    UConverterSharedData *mySharedConverterData;
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverterLoadArgs ರಾಶಿ_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_PACKAGE);

    ಯದಿ(U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ರಾಶಿ_Pieces.cnvName[0] = 0;
    ರಾಶಿ_Pieces.locale[0] = 0;
    ರಾಶಿ_Pieces.options = 0;
    parseConverterOptions(converterName, &ರಾಶಿ_Pieces, &ರಾಶಿ_Args, err);
    ಯದಿ (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_STATUS(*err);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ರಾಶಿ_Args.nestedLoads=1;
    ರಾಶಿ_Args.pkg=packageName;

    /* open the data, unflatten the shared structure */
    mySharedConverterData = createConverterFromFile(&ರಾಶಿ_Args, err);

    ಯದಿ (U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* create the actual converter */
    myUConverter = ucnv_createConverterFromSharedData(nullptr, mySharedConverterData, &ರಾಶಿ_Args, err);

    ಯದಿ (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_STATUS(*err);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
    ಪ್ರತ್ಯಾಗಮ myUConverter;
}


U_CFUNC UConverter*
ucnv_createConverterFromSharedData(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverterSharedData* mySharedConverterData,
                                   UConverterLoadArgs* pArgs,
                                   UErrorCode* err)
{
    UBool isCopyLocal;

    ಯದಿ(U_FAILURE(*err)) {
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
        ಪ್ರತ್ಯಾಗಮ myUConverter;
    }
    ಯದಿ(myUConverter == nullptr)
    {
        myUConverter = (UConverter *) uprv_malloc (ಪರಿಮಾಣ (UConverter));
        ಯದಿ(myUConverter == nullptr)
        {
            *err = U_MEMORY_ALLOCATION_ERROR;
            ucnv_unloadSharedDataIfReady(mySharedConverterData);
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        isCopyLocal = ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ {
        isCopyLocal = ಸತ್ಯ;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, ಪರಿಮಾಣ(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->sharedData = mySharedConverterData;
    myUConverter->options = pArgs->options;
    ಯದಿ(!pArgs->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_TO_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->toUnicodeStatus = mySharedConverterData->toUnicodeStatus;
        myUConverter->maxBytesPerUChar = mySharedConverterData->staticData->maxBytesPerChar;
        myUConverter->subChar1 = mySharedConverterData->staticData->subChar1;
        myUConverter->subCharLen = mySharedConverterData->staticData->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, mySharedConverterData->staticData->subChar, myUConverter->subCharLen);
        myUConverter->toUCallbackReason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    ಯದಿ(mySharedConverterData->impl->open != nullptr) {
        mySharedConverterData->impl->open(myUConverter, pArgs, err);
        ಯದಿ(U_FAILURE(*err) && !pArgs->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
    }

    ಪ್ರತ್ಯಾಗಮ myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverterSharedData *mySharedData = nullptr;
    int32_t pos;
    int32_t table_Deleted_ಅಂಕೆ = 0;
    ಧ್ರುವ UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, remaining;

    UTRACE_ENTRY_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    ಯದಿ (SHARED_DATA_HASHTABLE == nullptr) {
        UTRACE_EXIT_ಮೌಲ್ಯ((int32_t)0);
        ಪ್ರತ್ಯಾಗಮ 0;
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
    umtx_lock(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    ಕುರು {
        remaining = 0;
        pos = UHASH_FIRST;
        ಯಾವತ್ ((e = uhash_nextElement (SHARED_DATA_HASHTABLE, &pos)) != nullptr)
        {
            mySharedData = (UConverterSharedData *) e->value.pointer;
            /*deletes only if reference counter == 0 */
            ಯದಿ (mySharedData->referenceCounter == 0)
            {
                table_Deleted_ಅಂಕೆ++;

                UCNV_DEBUG_LOG("del",mySharedData->staticData->name,mySharedData);

                uhash_removeElement(SHARED_DATA_HASHTABLE, e);
                mySharedData->sharedDataCached = ಅಸತ್ಯ;
                ucnv_deleteSharedConverterData (mySharedData);
            } ಅನ್ಯಥಾ {
                ++remaining;
            }
        }
    } ಯಾವತ್(++i == 1 && remaining > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", remaining);

    UTRACE_EXIT_ಮೌಲ್ಯ(table_Deleted_ಅಂಕೆ);
    ಪ್ರತ್ಯಾಗಮ table_Deleted_ಅಂಕೆ;
}

/* available converters list --------------------------------------------------- */

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV initAvailableConvertersList(UErrorCode &errCode) {
    U_ASSERT(g_Available_Converter_ಎಣಿಕೆ == 0);
    U_ASSERT(gAvailableConverters == nullptr);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_openAllNames(&errCode);
    int32_t all_Converter_ಎಣಿಕೆ = uenum_ಎಣಿಕೆ(allConvEnum, &errCode);
    ಯದಿ (U_FAILURE(errCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* We can't have more than "*converterTable" converters to open */
    gAvailableConverters = static_cast<ಧ್ರುವ ವರ್ಣ**>(uprv_malloc(all_Converter_ಎಣಿಕೆ * ಪರಿಮಾಣ(ವರ್ಣ*)));
    ಯದಿ (!gAvailableConverters) {
        errCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UErrorCode localStatus = U_ZERO_ERROR;
    UConverter tempConverter;
    ucnv_close(ucnv_createConverter(&tempConverter, nullptr, &localStatus));

    g_Available_Converter_ಎಣಿಕೆ = 0;

    ಪ್ರತಿ (int32_t idx = 0; idx < all_Converter_ಎಣಿಕೆ; idx++) {
        localStatus = U_ZERO_ERROR;
        ಧ್ರುವ ವರ್ಣ *converterName = uenum_next(allConvEnum, nullptr, &localStatus);
        ಯದಿ (ucnv_canCreateConverter(converterName, &localStatus)) {
            gAvailableConverters[g_Available_Converter_ಎಣಿಕೆ++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


ಸ್ಥಿರ UBool haveAvailableConverterList(UErrorCode *pErrorCode) {
    umtx_initOnce(gAvailableConvertersInitOnce, &initAvailableConvertersList, *pErrorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(*pErrorCode);
}

U_CFUNC uint16_t
ucnv_bld_ಎಣಿಕೆ_Available_Converters(UErrorCode *pErrorCode) {
    ಯದಿ (haveAvailableConverterList(pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ g_Available_Converter_ಎಣಿಕೆ;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

U_CFUNC ಧ್ರುವ ವರ್ಣ *
ucnv_bld_getAvailableConverter(uint16_t n, UErrorCode *pErrorCode) {
    ಯದಿ (haveAvailableConverterList(pErrorCode)) {
        ಯದಿ (n < g_Available_Converter_ಎಣಿಕೆ) {
            ಪ್ರತ್ಯಾಗಮ gAvailableConverters[n];
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

/* default converter name --------------------------------------------------- */

#ಯದಿ !U_CHARSET_IS_UTF8
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
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
internalSetName(ಧ್ರುವ ವರ್ಣ *name, UErrorCode *status) {
    UConverterNamePieces ರಾಶಿ_Pieces;
    UConverterLoadArgs ರಾಶಿ_Args=UCNV_LOAD_ARGS_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool containsOption = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != nullptr);
    ಧ್ರುವ UConverterSharedData *algorithmicSharedData;

    ರಾಶಿ_Args.name = name;
    ಯದಿ(containsOption) {
        ರಾಶಿ_Pieces.cnvName[0] = 0;
        ರಾಶಿ_Pieces.locale[0] = 0;
        ರಾಶಿ_Pieces.options = 0;
        parseConverterOptions(name, &ರಾಶಿ_Pieces, &ರಾಶಿ_Args, status);
        ಯದಿ(U_FAILURE(*status)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
    algorithmicSharedData = getAlgorithmicTypeFromName(ರಾಶಿ_Args.name);

    umtx_lock(&cnvCacheMutex);

    gDefaultAlgorithmicSharedData = algorithmicSharedData;
    gDefaultConverterContainsOption = containsOption;
    uprv_memcpy(gDefaultConverterNameBuffer, name, length);
    gDefaultConverterNameBuffer[length]=0;

    /* gDefaultConverterName MUST be the last global var set by this function.  */
    /*    It is the variable checked in ucnv_getDefaultName() to see if initialization is required. */
    //    But there is nothing here preventing that from being reordered, either by the compiler
    //             or hardware. I'm adding the mutex to ucnv_getDefaultName for now. UMTX_CHECK is not enough.
    //             -- Andy
    gDefaultConverterName = gDefaultConverterNameBuffer;

    ucnv_enableCleanup();

    umtx_unlock(&cnvCacheMutex);
}
#endif

/*
 * In order to be really thread-safe, the get function would have to take
 * a buffer parameter and copy the current string inside a mutex block.
 * This implementation only tries to be really thread-safe while
 * setting the name.
 * It assumes that setting a pointer is atomic.
 */

U_CAPI ಧ್ರುವ ವರ್ಣ*  U_EXPORT2
ucnv_getDefaultName() {
#ಯದಿ U_CHARSET_IS_UTF8
    ಪ್ರತ್ಯಾಗಮ "UTF-8";
#ಅನ್ಯಥಾ
    /* local variable to be thread-safe */
    ಧ್ರುವ ವರ್ಣ *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex lock(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    ಯದಿ(name==nullptr) {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        ಯದಿ(name != nullptr) {
            cnv = ucnv_open(name, &errorCode);
            ಯದಿ(U_SUCCESS(errorCode) && cnv != nullptr) {
                name = ucnv_getName(cnv, &errorCode);
            }
        }

        ಯದಿ(name == nullptr || name[0] == 0
            || U_FAILURE(errorCode) || cnv == nullptr
            || uprv_strlen(name)>=ಪರಿಮಾಣ(gDefaultConverterNameBuffer))
        {
            /* Panic time, let's use a fallback. */
#ಯದಿ (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#ನೋಚೇದ್ಯದಿ U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_SWAP_LFNL_OPTION_STRING;
#ಅನ್ಯಥಾ
            name = "ibm-37_P100-1995";
#endif
        }

        internalSetName(name, &errorCode);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    ಪ್ರತ್ಯಾಗಮ name;
#endif
}

#ಯದಿ U_CHARSET_IS_UTF8
U_CAPI ಶೂನ್ಯ U_EXPORT2 ucnv_setDefaultName(ಧ್ರುವ ವರ್ಣ *) {}
#ಅನ್ಯಥಾ
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_setDefaultName(ಧ್ರುವ ವರ್ಣ *converterName) {
    ಯದಿ(converterName==nullptr) {
        /* reset to the default codepage */
        gDefaultConverterName=nullptr;
    } ಅನ್ಯಥಾ {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;
        ಧ್ರುವ ವರ್ಣ *name = nullptr;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &errorCode);
        ಯದಿ(U_SUCCESS(errorCode) && cnv != nullptr) {
            name = ucnv_getName(cnv, &errorCode);
        }

        ಯದಿ(U_SUCCESS(errorCode) && name!=nullptr) {
            internalSetName(name, &errorCode);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#endif

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#ಯದಿ !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_swap(ಧ್ರುವ UDataSwapper *ds,
          ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
          UErrorCode *pErrorCode) {
    ಧ್ರುವ UDataInfo *pInfo;
    int32_t headerSize;

    ಧ್ರುವ uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t offset, count, staticDataSize;
    int32_t size;

    ಧ್ರುವ UConverterStaticData *inStaticData;
    UConverterStaticData *outStaticData;

    ಧ್ರುವ _MBCSHeader *inMBCSHeader;
    _MBCSHeader *outMBCSHeader;
    _MBCSHeader mbcsHeader;
    uint32_t mbcsHeaderLength;
    UBool noFromU=ಅಸತ್ಯ;

    uint8_t outputType;

    int32_t maxFastUChar, mbcsIndexLength;

    ಧ್ರುವ int32_t *inExtIndexes;
    int32_t extOffset;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* check data format and format version */
    pInfo=(ಧ್ರುವ UDataInfo *)((ಧ್ರುವ ವರ್ಣ *)inData+4);
    ಯದಿ(!(
        pInfo->dataFormat[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x76 &&
        pInfo->dataFormat[3]==0x74 &&
        pInfo->formatVersion[0]==6 &&
        pInfo->formatVersion[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0], pInfo->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    inBytes=(ಧ್ರುವ uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    inStaticData=(ಧ್ರುವ UConverterStaticData *)inBytes;
    outStaticData=(UConverterStaticData *)outBytes;

    ಯದಿ(length<0) {
        staticDataSize=ds->readUInt32(inStaticData->structSize);
    } ಅನ್ಯಥಾ {
        length-=headerSize;
        ಯದಿ( length<(int32_t)ಪರಿಮಾಣ(UConverterStaticData) ||
            (uint32_t)length<(staticDataSize=ds->readUInt32(inStaticData->structSize))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }

    ಯದಿ(length>=0) {
        /* swap the static data */
        ಯದಿ(inStaticData!=outStaticData) {
            uprv_memcpy(outStaticData, inStaticData, staticDataSize);
        }

        ds->swapArray32(ds, &inStaticData->structSize, 4,
                           &outStaticData->structSize, pErrorCode);
        ds->swapArray32(ds, &inStaticData->codepage, 4,
                           &outStaticData->codepage, pErrorCode);

        ds->swapInvChars(ds, inStaticData->name, (int32_t)uprv_strlen(inStaticData->name),
                            outStaticData->name, pErrorCode);
        ಯದಿ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }

    inBytes+=staticDataSize;
    ಯದಿ (outBytes != nullptr) outBytes+=staticDataSize;
    ಯದಿ(length>=0) {
        length-=(int32_t)staticDataSize;
    }

    /* check for supported conversionType values */
    ಯದಿ(inStaticData->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        inMBCSHeader=(ಧ್ರುವ _MBCSHeader *)inBytes;
        outMBCSHeader=(_MBCSHeader *)outBytes;

        ಯದಿ(0<=length && length<(int32_t)ಪರಿಮಾಣ(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ(inMBCSHeader->version[0]==4 && inMBCSHeader->version[1]>=1) {
            mbcsHeaderLength=MBCS_HEADER_V4_LENGTH;
        } ಅನ್ಯಥಾ ಯದಿ(inMBCSHeader->version[0]==5 && inMBCSHeader->version[1]>=3 &&
                  ((mbcsHeader.options=ds->readUInt32(inMBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_MASK)==0
        ) {
            mbcsHeaderLength=mbcsHeader.options&MBCS_OPT_LENGTH_MASK;
            noFromU = (mbcsHeader.options & MBCS_OPT_NO_FROM_U) != 0;
        } ಅನ್ಯಥಾ {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             inMBCSHeader->version[0], inMBCSHeader->version[1]);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        uprv_memcpy(mbcsHeader.version, inMBCSHeader->version, 4);
        mbcsHeader.ಎಣಿಕೆ_States=         ds->readUInt32(inMBCSHeader->ಎಣಿಕೆ_States);
        mbcsHeader.ಎಣಿಕೆ_To_UFallbacks=   ds->readUInt32(inMBCSHeader->ಎಣಿಕೆ_To_UFallbacks);
        mbcsHeader.offsetToUCodeUnits=  ds->readUInt32(inMBCSHeader->offsetToUCodeUnits);
        mbcsHeader.offsetFromUTable=    ds->readUInt32(inMBCSHeader->offsetFromUTable);
        mbcsHeader.offsetFromUBytes=    ds->readUInt32(inMBCSHeader->offsetFromUBytes);
        mbcsHeader.flags=               ds->readUInt32(inMBCSHeader->flags);
        mbcsHeader.fromUBytesLength=    ds->readUInt32(inMBCSHeader->fromUBytesLength);
        /* mbcsHeader.options have been read above */

        extOffset=(int32_t)(mbcsHeader.flags>>8);
        outputType=(uint8_t)mbcsHeader.flags;
        ಯದಿ(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* make sure that the output type is known */
        ವಿಕಲ್ಪ(outputType) {
        ಪಕ್ಷ MBCS_OUTPUT_1:
        ಪಕ್ಷ MBCS_OUTPUT_2:
        ಪಕ್ಷ MBCS_OUTPUT_3:
        ಪಕ್ಷ MBCS_OUTPUT_4:
        ಪಕ್ಷ MBCS_OUTPUT_3_EUC:
        ಪಕ್ಷ MBCS_OUTPUT_4_EUC:
        ಪಕ್ಷ MBCS_OUTPUT_2_SISO:
        ಪಕ್ಷ MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* calculate the length of the MBCS data */

        /*
         * utf8Friendly MBCS files (mbcsHeader.version 4.3)
         * contain an additional mbcsIndex table:
         *   uint16_t[(maxFastUChar+1)>>6];
         * where maxFastUChar=((mbcsHeader.version[2]<<8)|0xff).
         */
        maxFastUChar=0;
        mbcsIndexLength=0;
        ಯದಿ( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcsHeader.version[1]>=3 && (maxFastUChar=mbcsHeader.version[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcsIndexLength=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        ಯದಿ(extOffset==0) {
            size=(int32_t)(mbcsHeader.offsetFromUBytes+mbcsIndexLength);
            ಯದಿ(!noFromU) {
                size+=(int32_t)mbcsHeader.fromUBytesLength;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            inExtIndexes=nullptr;
        } ಅನ್ಯಥಾ {
            /* there is extension data after the base data, see ucnv_ext.h */
            ಯದಿ(length>=0 && length<(extOffset+UCNV_EXT_INDEXES_MIN_LENGTH*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }

            inExtIndexes=(ಧ್ರುವ int32_t *)(inBytes+extOffset);
            size=extOffset+udata_readInt32(ds, inExtIndexes[UCNV_EXT_SIZE]);
        }

        ಯದಿ(length>=0) {
            ಯದಿ(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }

            /* copy the data for inaccessible bytes */
            ಯದಿ(inBytes!=outBytes) {
                uprv_memcpy(outBytes, inBytes, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcsHeaderLength*4;
            ds->swapArray32(ds, &inMBCSHeader->ಎಣಿಕೆ_States, count-4,
                               &outMBCSHeader->ಎಣಿಕೆ_States, pErrorCode);

            ಯದಿ(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ಧ್ರುವ ವರ್ಣ *inBaseName=(ಧ್ರುವ ವರ್ಣ *)inBytes+count;
                ವರ್ಣ *outBaseName=(ವರ್ಣ *)outBytes+count;
                ds->swapInvChars(ds, inBaseName, (int32_t)uprv_strlen(inBaseName),
                                    outBaseName, pErrorCode);
            } ಅನ್ಯಥಾ {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                offset=count;
                count=mbcsHeader.ಎಣಿಕೆ_States*1024;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the toUFallbacks[] */
                offset+=count;
                count=mbcsHeader.ಎಣಿಕೆ_To_UFallbacks*8;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the unicodeCodeUnits[] */
                offset=mbcsHeader.offsetToUCodeUnits;
                count=mbcsHeader.offsetFromUTable-offset;
                ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* offset to the stage 1 table, independent of the outputType */
                offset=mbcsHeader.offsetFromUTable;

                ಯದಿ(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcsHeader.offsetFromUBytes-offset)+mbcsHeader.fromUBytesLength;
                    ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                       outBytes+offset, pErrorCode);
                } ಅನ್ಯಥಾ {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    ಯದಿ(inStaticData->unicodeMask&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } ಅನ್ಯಥಾ {
                        count=0x40*2; /* only BMP */
                    }
                    ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                       outBytes+offset, pErrorCode);

                    /* stage 2 table: uint32_t[] */
                    offset+=count;
                    count=mbcsHeader.offsetFromUBytes-offset;
                    ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                       outBytes+offset, pErrorCode);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    offset=mbcsHeader.offsetFromUBytes;
                    count= noFromU ? 0 : mbcsHeader.fromUBytesLength;
                    ವಿಕಲ್ಪ(outputType) {
                    ಪಕ್ಷ MBCS_OUTPUT_2:
                    ಪಕ್ಷ MBCS_OUTPUT_3_EUC:
                    ಪಕ್ಷ MBCS_OUTPUT_2_SISO:
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        ಭಙ್ಗ;
                    ಪಕ್ಷ MBCS_OUTPUT_4:
                        ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        ಭಙ್ಗ;
                    ಸ್ವಭಾವ:
                        /* just uint8_t[], nothing to swap */
                        ಭಙ್ಗ;
                    }

                    ಯದಿ(mbcsIndexLength!=0) {
                        offset+=count;
                        count=mbcsIndexLength;
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                    }
                }
            }

            ಯದಿ(extOffset!=0) {
                /* swap the extension data */
                inBytes+=extOffset;
                outBytes+=extOffset;

                /* swap toUTable[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_INDEX]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_LENGTH]);
                ds->swapArray32(ds, inBytes+offset, length*4, outBytes+offset, pErrorCode);

                /* swap toUUChars[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_UCHARS_INDEX]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_UCHARS_LENGTH]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUTableUChars[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_UCHARS_INDEX]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_LENGTH]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUTableValues[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_ಮೌಲ್ಯಗಳು_INDEX]);
                /* same length as for fromUTableUChars[] */
                ds->swapArray32(ds, inBytes+offset, length*4, outBytes+offset, pErrorCode);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_12_INDEX]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_12_LENGTH]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUStage3[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_INDEX]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_LENGTH]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUStage3b[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3B_INDEX]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3B_LENGTH]);
                ds->swapArray32(ds, inBytes+offset, length*4, outBytes+offset, pErrorCode);

                /* swap indexes[] */
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_INDEXES_LENGTH]);
                ds->swapArray32(ds, inBytes, length*4, outBytes, pErrorCode);
            }
        }
    } ಅನ್ಯಥಾ {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         inStaticData->conversionType);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪ್ರತ್ಯಾಗಮ headerSize+(int32_t)staticDataSize+size;
}

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#endif
