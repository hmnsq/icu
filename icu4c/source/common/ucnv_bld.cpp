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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "utracimp.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_io.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_bld.h"
#ଅନ୍ତର୍ଭାଵ "ucnvmbcs.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_ext.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_cnv.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_imp.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "ustr_cnv.h"


#ଯଦି 0
#ଅନ୍ତର୍ଭାଵ <stdio.h>
ବାହ୍ୟ ଶୂନ୍ୟ UCNV_DEBUG_LOG(ଅକ୍ଷର *what, ଅକ୍ଷର *who, ଶୂନ୍ୟ *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା l);
#define UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#ଅନ୍ୟଥା
# define UCNV_DEBUG_LOG(x,y,z)
#endif

ସ୍ଥିର ଅଚଳ UConverterSharedData * ଅଚଳ
converterData[UCNV_ସଂଖ୍ୟା_OF_SUPPORTED_CONVERTER_TYPES]={
    nullptr, nullptr,

#ଯଦି UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#ଅନ୍ୟଥା
    &_MBCSData,
#endif

    &_Latin1Data,
    &_UTF8Data, &_UTF16BEData, &_UTF16LEData,
#ଯଦି UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr,
#ଅନ୍ୟଥା
    &_UTF32BEData, &_UTF32LEData,
#endif
    nullptr,

#ଯଦି UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#ଅନ୍ୟଥା
    &_ISO2022Data,
#endif

#ଯଦି UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr,
#ଅନ୍ୟଥା
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#endif

#ଯଦି UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#ଅନ୍ୟଥା
    &_SCSUData,
#endif


#ଯଦି UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#ଅନ୍ୟଥା
    &_ISCIIData,
#endif

    &_ASCIIData,
#ଯଦି UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, &_UTF16Data, nullptr, nullptr, nullptr,
#ଅନ୍ୟଥା
    &_UTF7Data, &_Bocu1Data, &_UTF16Data, &_UTF32Data, &_CESU8Data, &_IMAPData,
#endif

#ଯଦି UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#ଅନ୍ୟଥା
    &_CompoundTextData
#endif
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
ସ୍ଥିର ସଂରଚନା {
  ଅଚଳ ଅକ୍ଷର *name;
  ଅଚଳ UConverterType type;
} ଅଚଳ cnvNameType[] = {
#ଯଦି !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#endif
#ଯଦି !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#endif
#ଯଦି !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#endif
#ଯଦି !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#endif
#ଯଦି !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#endif
  { "iso88591", UCNV_LATIN_1 },
#ଯଦି !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#ଯଦି !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#endif
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#ଯଦି U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#ଅନ୍ୟଥା
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#endif
#ଯଦି !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#ଯଦି U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#ଅନ୍ୟଥା
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#endif
#endif
#ଯଦି !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#endif
  { "utf8", UCNV_UTF8 },
#ଯଦି !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_TEXT}
#endif
};


/*initializes some global variables */
ସ୍ଥିର UHashtable *SHARED_DATA_HASHTABLE = nullptr;
ସ୍ଥିର icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର **gAvailableConverters = nullptr;
ସ୍ଥିର uint16_t g_Available_Converter_ଗଣନା = 0;
ସ୍ଥିର icu::UInitOnce gAvailableConvertersInitOnce {};

#ଯଦି !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
ସ୍ଥିର ଅକ୍ଷର gDefaultConverterNameBuffer[UCNV_MAX_CONVERTER_NAME_LENGTH + 1]; /* +1 for nullptr */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *gDefaultConverterName = nullptr;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
ସ୍ଥିର ଅଚଳ UConverterSharedData *gDefaultAlgorithmicSharedData = nullptr;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
ସ୍ଥିର UBool gDefaultConverterContainsOption;

#endif  /* !U_CHARSET_IS_UTF8 */

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର DATA_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
ସ୍ଥିର ଶୂନ୍ୟ
ucnv_flushAvailableConverterCache() {
    g_Available_Converter_ଗଣନା = 0;
    ଯଦି (gAvailableConverters) {
        uprv_free(const_cast<ଅକ୍ଷର**>(gAvailableConverters));
        gAvailableConverters = nullptr;
    }
    gAvailableConvertersInitOnce.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
ସ୍ଥିର UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    ଯଦି (SHARED_DATA_HASHTABLE != nullptr && uhash_ଗଣନା(SHARED_DATA_HASHTABLE) == 0) {
        uhash_close(SHARED_DATA_HASHTABLE);
        SHARED_DATA_HASHTABLE = nullptr;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flushAvailableConverterCache();

#ଯଦି !U_CHARSET_IS_UTF8
    gDefaultConverterName = nullptr;
    gDefaultConverterNameBuffer[0] = 0;
    gDefaultConverterContainsOption = ମିଥ୍ୟା;
    gDefaultAlgorithmicSharedData = nullptr;
#endif

    ଫେରନ୍ତୁ (SHARED_DATA_HASHTABLE == nullptr);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_registerCleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

ସ୍ଥିର UBool U_CALLCONV
isCnvAcceptable(ଶୂନ୍ୟ * /*context*/,
                ଅଚଳ ଅକ୍ଷର * /*type*/, ଅଚଳ ଅକ୍ଷର * /*name*/,
                ଅଚଳ UDataInfo *pInfo) {
    ଫେରନ୍ତୁ
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
ସ୍ଥିର UConverterSharedData*
ucnv_data_unFlattenClone(UConverterLoadArgs *pArgs, UDataMemory *pData, UErrorCode *status)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ଅଚଳ uint8_t* raw = static_cast<ଅଚଳ uint8_t*>(udata_getMemory(pData));
    ଅଚଳ UConverterStaticData* source = reinterpret_cast<ଅଚଳ UConverterStaticData*>(raw);
    UConverterSharedData *data;
    UConverterType type = static_cast<UConverterType>(source->conversionType);

    ଯଦି(U_FAILURE(*status))
        ଫେରନ୍ତୁ nullptr;

    ଯଦି (static_cast<uint16_t>(type) >= UCNV_ସଂଖ୍ୟା_OF_SUPPORTED_CONVERTER_TYPES ||
        converterData[type] == nullptr ||
        !converterData[type]->isReferenceCounted ||
        converterData[type]->referenceCounter != 1 ||
        source->structSize != ଆକାର(UConverterStaticData))
    {
        *status = U_INVALID_TABLE_FORMAT;
        ଫେରନ୍ତୁ nullptr;
    }

    data = static_cast<UConverterSharedData*>(uprv_malloc(ଆକାର(UConverterSharedData)));
    ଯଦି(data == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(data, converterData[type], ଆକାର(UConverterSharedData));

    data->staticData = source;

    data->sharedDataCached = ମିଥ୍ୟା;

    /* fill in fields from the loaded data */
    data->dataMemory = (ଶୂନ୍ୟ*)pData; /* for future use */

    ଯଦି(data->impl->load != nullptr) {
        data->impl->load(data, pArgs, raw + source->structSize, status);
        ଯଦି(U_FAILURE(*status)) {
            uprv_free(data);
            ଫେରନ୍ତୁ nullptr;
        }
    }
    ଫେରନ୍ତୁ data;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
ସ୍ଥିର UConverterSharedData *createConverterFromFile(UConverterLoadArgs *pArgs, UErrorCode * err)
{
    UDataMemory *data;
    UConverterSharedData *sharedData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_LOAD);

    ଯଦି (U_FAILURE (*err)) {
        UTRACE_EXIT_STATUS(*err);
        ଫେରନ୍ତୁ nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", pArgs->name, pArgs->pkg);

    data = udata_openChoice(pArgs->pkg, DATA_TYPE, pArgs->name, isCnvAcceptable, nullptr, err);
    ଯଦି(U_FAILURE(*err))
    {
        UTRACE_EXIT_STATUS(*err);
        ଫେରନ୍ତୁ nullptr;
    }

    sharedData = ucnv_data_unFlattenClone(pArgs, data, err);
    ଯଦି(U_FAILURE(*err))
    {
        udata_close(data);
        UTRACE_EXIT_STATUS(*err);
        ଫେରନ୍ତୁ nullptr;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_STATUS(sharedData, *err);
    ଫେରନ୍ତୁ sharedData;
}

/*returns a converter type from a string
 */
ସ୍ଥିର ଅଚଳ UConverterSharedData *
getAlgorithmicTypeFromName(ଅଚଳ ଅକ୍ଷର *realName)
{
    uint32_t mid, start, limit;
    uint32_t lastMid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା result;
    ଅକ୍ଷର strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    start = 0;
    limit = UPRV_LENGTHOF(cnvNameType);
    mid = limit;
    lastMid = UINT32_MAX;

    ପାଇଁ (;;) {
        mid = (start + limit) / 2;
        ଯଦି (lastMid == mid) {   /* Have we moved? */
            ଭାଙ୍ଗ;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        result = uprv_strcmp(strippedName, cnvNameType[mid].name);

        ଯଦି (result < 0) {
            limit = mid;
        } ଅନ୍ୟଥା ଯଦି (result > 0) {
            start = mid;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ converterData[cnvNameType[mid].type];
        }
    }

    ଫେରନ୍ତୁ nullptr;
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
ସ୍ଥିର ଶୂନ୍ୟ
ucnv_shareConverterData(UConverterSharedData * data)
{
    UErrorCode err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    ଯଦି (SHARED_DATA_HASHTABLE == nullptr)
    {
        SHARED_DATA_HASHTABLE = uhash_openSize(uhash_hashChars, uhash_compareChars, nullptr,
                            ucnv_io_ଗଣନା_Known_Converters(&err)*UCNV_CACHE_LOAD_FACTOR,
                            &err);
        ucnv_enableCleanup();

        ଯଦି (U_FAILURE(err))
            ଫେରନ୍ତୁ;
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
    data->sharedDataCached = ସତ୍ୟ;

    uhash_put(SHARED_DATA_HASHTABLE,
            (ଶୂନ୍ୟ*) data->staticData->name, /* Okay to cast away const as long as
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
ସ୍ଥିର UConverterSharedData *
ucnv_getSharedConverterData(ଅଚଳ ଅକ୍ଷର *name)
{
    /*special case when no Table has yet been created we return nullptr */
    ଯଦି (SHARED_DATA_HASHTABLE == nullptr)
    {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅନ୍ୟଥା
    {
        UConverterSharedData *rc;

        rc = static_cast<UConverterSharedData*>(uhash_get(SHARED_DATA_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        ଫେରନ୍ତୁ rc;
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
ସ୍ଥିର UBool
ucnv_deleteSharedConverterData(UConverterSharedData * deadSharedData)
{
    UTRACE_ENTRY_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", deadSharedData->staticData->name, deadSharedData);

    ଯଦି (deadSharedData->referenceCounter > 0) {
        UTRACE_EXIT_ମୂଲ୍ୟ((int32_t)ମିଥ୍ୟା);
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    ଯଦି (deadSharedData->impl->unload != nullptr) {
        deadSharedData->impl->unload(deadSharedData);
    }

    ଯଦି(deadSharedData->dataMemory != nullptr)
    {
        UDataMemory *data = (UDataMemory*)deadSharedData->dataMemory;
        udata_close(data);
    }

    uprv_free(deadSharedData);

    UTRACE_EXIT_ମୂଲ୍ୟ((int32_t)ସତ୍ୟ);
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverterSharedData *
ucnv_load(UConverterLoadArgs *pArgs, UErrorCode *err) {
    UConverterSharedData *mySharedConverterData;

    ଯଦି(err == nullptr || U_FAILURE(*err)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି(pArgs->pkg != nullptr && *pArgs->pkg != 0) {
        /* application-provided converters are not currently cached */
        ଫେରନ୍ତୁ createConverterFromFile(pArgs, err);
    }

    mySharedConverterData = ucnv_getSharedConverterData(pArgs->name);
    ଯଦି (mySharedConverterData == nullptr)
    {
        /*Not cached, we need to stream it in from file */
        mySharedConverterData = createConverterFromFile(pArgs, err);
        ଯଦି (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            ଫେରନ୍ତୁ nullptr;
        }
        ଅନ୍ୟଥା ଯଦି (!pArgs->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_shareConverterData(mySharedConverterData);
        }
    }
    ଅନ୍ୟଥା
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        mySharedConverterData->referenceCounter++;
    }

    ଫେରନ୍ତୁ mySharedConverterData;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI ଶୂନ୍ୟ
ucnv_unload(UConverterSharedData *sharedData) {
    ଯଦି(sharedData != nullptr) {
        ଯଦି (sharedData->referenceCounter > 0) {
            sharedData->referenceCounter--;
        }

        ଯଦି((sharedData->referenceCounter <= 0)&&(sharedData->sharedDataCached == ମିଥ୍ୟା)) {
            ucnv_deleteSharedConverterData(sharedData);
        }
    }
}

U_CFUNC ଶୂନ୍ୟ
ucnv_unloadSharedDataIfReady(UConverterSharedData *sharedData)
{
    ଯଦି(sharedData != nullptr && sharedData->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        ucnv_unload(sharedData);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC ଶୂନ୍ୟ
ucnv_increment_Ref_ଗଣନା(UConverterSharedData *sharedData)
{
    ଯଦି(sharedData != nullptr && sharedData->isReferenceCounted) {
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
ସ୍ଥିର ଶୂନ୍ୟ
parseConverterOptions(ଅଚଳ ଅକ୍ଷର *inName,
                      UConverterNamePieces *pPieces,
                      UConverterLoadArgs *pArgs,
                      UErrorCode *err)
{
    ଅକ୍ଷର *cnvName = pPieces->cnvName;
    ଅକ୍ଷର c;
    int32_t len = 0;

    pArgs->name=inName;
    pArgs->locale=pPieces->locale;
    pArgs->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    ଯେତେବେଳେ((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        ଯଦି (++len>=UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *err = U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            ଫେରନ୍ତୁ;
        }
        *cnvName++=c;
        inName++;
    }
    *cnvName=0;
    pArgs->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    ଯେତେବେଳେ((c=*inName)!=0) {
        ଯଦି(c==UCNV_OPTION_SEP_CHAR) {
            ++inName;
        }

        /* inName is behind an option separator */
        ଯଦି(uprv_strncmp(inName, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            ଅକ୍ଷର *dest=pPieces->locale;

            /* copy the locale option value */
            inName+=7;
            len=0;
            ଯେତେବେଳେ((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++inName;

                ଯଦି(++len>=ULOC_FULLNAME_CAPACITY) {
                    *err=U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    ଫେରନ୍ତୁ;
                }

                *dest++=c;
            }
            *dest=0;
        } ଅନ୍ୟଥା ଯଦି(uprv_strncmp(inName, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            inName+=8;
            c=*inName;
            ଯଦି(c==0) {
                pArgs->options=(pPieces->options&=~UCNV_OPTION_VERSION);
                ଫେରନ୍ତୁ;
            } ଅନ୍ୟଥା ଯଦି (static_cast<uint8_t>(c - '0') < 10) {
                pArgs->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_VERSION) | static_cast<uint32_t>(c - '0');
                ++inName;
            }
        } ଅନ୍ୟଥା ଯଦି(uprv_strncmp(inName, "swaplfnl", 8)==0) {
            inName+=8;
            pArgs->options=(pPieces->options|=UCNV_OPTION_SWAP_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } ଅନ୍ୟଥା {
            /* ignore any other options until we define some */
            ଯେତେବେଳେ(((c = *inName++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            ଯଦି(c==0) {
                ଫେରନ୍ତୁ;
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
ucnv_loadSharedData(ଅଚଳ ଅକ୍ଷର *converterName,
                    UConverterNamePieces *pPieces,
                    UConverterLoadArgs *pArgs,
                    UErrorCode * err) {
    UConverterNamePieces ଗଦା_Pieces;
    UConverterLoadArgs ଗଦା_Args;
    UConverterSharedData *mySharedConverterData = nullptr;
    UErrorCode internalErrorCode = U_ZERO_ERROR;
    UBool mayContainOption = ସତ୍ୟ;
    UBool checkForAlgorithmic = ସତ୍ୟ;

    ଯଦି (U_FAILURE (*err)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି(pPieces == nullptr) {
        ଯଦି(pArgs != nullptr) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_INTERNAL_PROGRAM_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        pPieces = &ଗଦା_Pieces;
    }
    ଯଦି(pArgs == nullptr) {
        uprv_memset(&ଗଦା_Args, 0, ଆକାର(ଗଦା_Args));
        ଗଦା_Args.size = (int32_t)ଆକାର(ଗଦା_Args);
        pArgs = &ଗଦା_Args;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    pArgs->name = converterName;
    pArgs->locale = pPieces->locale;
    pArgs->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    ଯଦି (converterName == nullptr) {
#ଯଦି U_CHARSET_IS_UTF8
        pArgs->name = "UTF-8";
        ଫେରନ୍ତୁ (UConverterSharedData *)converterData[UCNV_UTF8];
#ଅନ୍ୟଥା
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        pArgs->name = ucnv_getDefaultName();
        ଯଦି (pArgs->name == nullptr) {
            *err = U_MISSING_RESOURCE_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        mySharedConverterData = (UConverterSharedData *)gDefaultAlgorithmicSharedData;
        checkForAlgorithmic = ମିଥ୍ୟା;
        mayContainOption = gDefaultConverterContainsOption;
        /* the default converter name is already canonical */
#endif
    }
    ଅନ୍ୟଥା ଯଦି(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        pArgs->name = "UTF-8";
        ଫେରନ୍ତୁ (UConverterSharedData *)converterData[UCNV_UTF8];
    }
    ଅନ୍ୟଥା {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, pArgs, err);
        ଯଦି (U_FAILURE(*err)) {
            /* Very bad name used. */
            ଫେରନ୍ତୁ nullptr;
        }

        /* get the canonical converter name */
        pArgs->name = ucnv_io_getConverterName(pArgs->name, &mayContainOption, &internalErrorCode);
        ଯଦି (U_FAILURE(internalErrorCode) || pArgs->name == nullptr) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            pArgs->name = pPieces->cnvName;
        } ଅନ୍ୟଥା ଯଦି (internalErrorCode == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    ଯଦି(mayContainOption && pArgs->name != pPieces->cnvName) {
        parseConverterOptions(pArgs->name, pPieces, pArgs, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    ଯଦି (checkForAlgorithmic) {
        mySharedConverterData = (UConverterSharedData *)getAlgorithmicTypeFromName(pArgs->name);
    }
    ଯଦି (mySharedConverterData == nullptr)
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
        ଯଦି (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            ଫେରନ୍ତୁ nullptr;
        }
    }

    ଫେରନ୍ତୁ mySharedConverterData;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ଅଚଳ ଅକ୍ଷର* converterName,
                     UErrorCode* err)
{
    UConverterNamePieces ଗଦା_Pieces;
    UConverterLoadArgs ଗଦା_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    ଯଦି(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        mySharedConverterData = ucnv_loadSharedData(converterName, &ଗଦା_Pieces, &ଗଦା_Args, err);

        myUConverter = ucnv_createConverterFromSharedData(
            myUConverter, mySharedConverterData,
            &ଗଦା_Args,
            err);

        ଯଦି(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
            ଫେରନ୍ତୁ myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_STATUS(*err);
    ଫେରନ୍ତୁ nullptr;
}

U_CFUNC UBool
ucnv_canCreateConverter(ଅଚଳ ଅକ୍ଷର *converterName, UErrorCode *err) {
    UConverter myUConverter;
    UConverterNamePieces ଗଦା_Pieces;
    UConverterLoadArgs ଗଦା_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    ଯଦି(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ଗଦା_Args.onlyTestIsLoadable=ସତ୍ୟ;
        mySharedConverterData = ucnv_loadSharedData(converterName, &ଗଦା_Pieces, &ଗଦା_Args, err);
        ucnv_createConverterFromSharedData(
            &myUConverter, mySharedConverterData,
            &ଗଦା_Args,
            err);
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
    }

    UTRACE_EXIT_STATUS(*err);
    ଫେରନ୍ତୁ U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ଅଚଳ ଅକ୍ଷର* locale,
                                uint32_t options,
                                UErrorCode* err) {
    UConverter *cnv;
    ଅଚଳ UConverterSharedData *sharedData;
    UConverterLoadArgs ଗଦା_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    ଯଦି(type<0 || UCNV_ସଂଖ୍ୟା_OF_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        ଫେରନ୍ତୁ nullptr;
    }

    sharedData = converterData[type];
    ଯଦି(sharedData == nullptr || sharedData->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        ଫେରନ୍ତୁ nullptr;
    }

    ଗଦା_Args.name = "";
    ଗଦା_Args.options = options;
    ଗଦା_Args.locale=locale;
    cnv = ucnv_createConverterFromSharedData(
            myUConverter, const_cast<UConverterSharedData*>(sharedData),
            &ଗଦା_Args, err);

    UTRACE_EXIT_PTR_STATUS(cnv, *err);
    ଫେରନ୍ତୁ cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ଅଚଳ ଅକ୍ଷର *packageName, ଅଚଳ ଅକ୍ଷର *converterName, UErrorCode * err)
{
    UConverter *myUConverter;
    UConverterSharedData *mySharedConverterData;
    UConverterNamePieces ଗଦା_Pieces;
    UConverterLoadArgs ଗଦା_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_PACKAGE);

    ଯଦି(U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        ଫେରନ୍ତୁ nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ଗଦା_Pieces.cnvName[0] = 0;
    ଗଦା_Pieces.locale[0] = 0;
    ଗଦା_Pieces.options = 0;
    parseConverterOptions(converterName, &ଗଦା_Pieces, &ଗଦା_Args, err);
    ଯଦି (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_STATUS(*err);
        ଫେରନ୍ତୁ nullptr;
    }
    ଗଦା_Args.nestedLoads=1;
    ଗଦା_Args.pkg=packageName;

    /* open the data, unflatten the shared structure */
    mySharedConverterData = createConverterFromFile(&ଗଦା_Args, err);

    ଯଦି (U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        ଫେରନ୍ତୁ nullptr;
    }

    /* create the actual converter */
    myUConverter = ucnv_createConverterFromSharedData(nullptr, mySharedConverterData, &ଗଦା_Args, err);

    ଯଦି (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_STATUS(*err);
        ଫେରନ୍ତୁ nullptr;
    }

    UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
    ଫେରନ୍ତୁ myUConverter;
}


U_CFUNC UConverter*
ucnv_createConverterFromSharedData(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverterSharedData* mySharedConverterData,
                                   UConverterLoadArgs* pArgs,
                                   UErrorCode* err)
{
    UBool isCopyLocal;

    ଯଦି(U_FAILURE(*err)) {
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
        ଫେରନ୍ତୁ myUConverter;
    }
    ଯଦି(myUConverter == nullptr)
    {
        myUConverter = (UConverter *) uprv_malloc (ଆକାର (UConverter));
        ଯଦି(myUConverter == nullptr)
        {
            *err = U_MEMORY_ALLOCATION_ERROR;
            ucnv_unloadSharedDataIfReady(mySharedConverterData);
            ଫେରନ୍ତୁ nullptr;
        }
        isCopyLocal = ମିଥ୍ୟା;
    } ଅନ୍ୟଥା {
        isCopyLocal = ସତ୍ୟ;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, ଆକାର(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->sharedData = mySharedConverterData;
    myUConverter->options = pArgs->options;
    ଯଦି(!pArgs->onlyTestIsLoadable) {
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

    ଯଦି(mySharedConverterData->impl->open != nullptr) {
        mySharedConverterData->impl->open(myUConverter, pArgs, err);
        ଯଦି(U_FAILURE(*err) && !pArgs->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            ଫେରନ୍ତୁ nullptr;
        }
    }

    ଫେରନ୍ତୁ myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverterSharedData *mySharedData = nullptr;
    int32_t pos;
    int32_t table_Deleted_ଅଙ୍କ = 0;
    ଅଚଳ UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, remaining;

    UTRACE_ENTRY_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    ଯଦି (SHARED_DATA_HASHTABLE == nullptr) {
        UTRACE_EXIT_ମୂଲ୍ୟ((int32_t)0);
        ଫେରନ୍ତୁ 0;
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
    କର {
        remaining = 0;
        pos = UHASH_FIRST;
        ଯେତେବେଳେ ((e = uhash_nextElement (SHARED_DATA_HASHTABLE, &pos)) != nullptr)
        {
            mySharedData = (UConverterSharedData *) e->value.pointer;
            /*deletes only if reference counter == 0 */
            ଯଦି (mySharedData->referenceCounter == 0)
            {
                table_Deleted_ଅଙ୍କ++;

                UCNV_DEBUG_LOG("del",mySharedData->staticData->name,mySharedData);

                uhash_removeElement(SHARED_DATA_HASHTABLE, e);
                mySharedData->sharedDataCached = ମିଥ୍ୟା;
                ucnv_deleteSharedConverterData (mySharedData);
            } ଅନ୍ୟଥା {
                ++remaining;
            }
        }
    } ଯେତେବେଳେ(++i == 1 && remaining > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", remaining);

    UTRACE_EXIT_ମୂଲ୍ୟ(table_Deleted_ଅଙ୍କ);
    ଫେରନ୍ତୁ table_Deleted_ଅଙ୍କ;
}

/* available converters list --------------------------------------------------- */

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV initAvailableConvertersList(UErrorCode &errCode) {
    U_ASSERT(g_Available_Converter_ଗଣନା == 0);
    U_ASSERT(gAvailableConverters == nullptr);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_openAllNames(&errCode);
    int32_t all_Converter_ଗଣନା = uenum_ଗଣନା(allConvEnum, &errCode);
    ଯଦି (U_FAILURE(errCode)) {
        ଫେରନ୍ତୁ;
    }

    /* We can't have more than "*converterTable" converters to open */
    gAvailableConverters = static_cast<ଅଚଳ ଅକ୍ଷର**>(uprv_malloc(all_Converter_ଗଣନା * ଆକାର(ଅକ୍ଷର*)));
    ଯଦି (!gAvailableConverters) {
        errCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UErrorCode localStatus = U_ZERO_ERROR;
    UConverter tempConverter;
    ucnv_close(ucnv_createConverter(&tempConverter, nullptr, &localStatus));

    g_Available_Converter_ଗଣନା = 0;

    ପାଇଁ (int32_t idx = 0; idx < all_Converter_ଗଣନା; idx++) {
        localStatus = U_ZERO_ERROR;
        ଅଚଳ ଅକ୍ଷର *converterName = uenum_next(allConvEnum, nullptr, &localStatus);
        ଯଦି (ucnv_canCreateConverter(converterName, &localStatus)) {
            gAvailableConverters[g_Available_Converter_ଗଣନା++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


ସ୍ଥିର UBool haveAvailableConverterList(UErrorCode *pErrorCode) {
    umtx_initOnce(gAvailableConvertersInitOnce, &initAvailableConvertersList, *pErrorCode);
    ଫେରନ୍ତୁ U_SUCCESS(*pErrorCode);
}

U_CFUNC uint16_t
ucnv_bld_ଗଣନା_Available_Converters(UErrorCode *pErrorCode) {
    ଯଦି (haveAvailableConverterList(pErrorCode)) {
        ଫେରନ୍ତୁ g_Available_Converter_ଗଣନା;
    }
    ଫେରନ୍ତୁ 0;
}

U_CFUNC ଅଚଳ ଅକ୍ଷର *
ucnv_bld_getAvailableConverter(uint16_t n, UErrorCode *pErrorCode) {
    ଯଦି (haveAvailableConverterList(pErrorCode)) {
        ଯଦି (n < g_Available_Converter_ଗଣନା) {
            ଫେରନ୍ତୁ gAvailableConverters[n];
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }
    ଫେରନ୍ତୁ nullptr;
}

/* default converter name --------------------------------------------------- */

#ଯଦି !U_CHARSET_IS_UTF8
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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
internalSetName(ଅଚଳ ଅକ୍ଷର *name, UErrorCode *status) {
    UConverterNamePieces ଗଦା_Pieces;
    UConverterLoadArgs ଗଦା_Args=UCNV_LOAD_ARGS_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool containsOption = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != nullptr);
    ଅଚଳ UConverterSharedData *algorithmicSharedData;

    ଗଦା_Args.name = name;
    ଯଦି(containsOption) {
        ଗଦା_Pieces.cnvName[0] = 0;
        ଗଦା_Pieces.locale[0] = 0;
        ଗଦା_Pieces.options = 0;
        parseConverterOptions(name, &ଗଦା_Pieces, &ଗଦା_Args, status);
        ଯଦି(U_FAILURE(*status)) {
            ଫେରନ୍ତୁ;
        }
    }
    algorithmicSharedData = getAlgorithmicTypeFromName(ଗଦା_Args.name);

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

U_CAPI ଅଚଳ ଅକ୍ଷର*  U_EXPORT2
ucnv_getDefaultName() {
#ଯଦି U_CHARSET_IS_UTF8
    ଫେରନ୍ତୁ "UTF-8";
#ଅନ୍ୟଥା
    /* local variable to be thread-safe */
    ଅଚଳ ଅକ୍ଷର *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex lock(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    ଯଦି(name==nullptr) {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        ଯଦି(name != nullptr) {
            cnv = ucnv_open(name, &errorCode);
            ଯଦି(U_SUCCESS(errorCode) && cnv != nullptr) {
                name = ucnv_getName(cnv, &errorCode);
            }
        }

        ଯଦି(name == nullptr || name[0] == 0
            || U_FAILURE(errorCode) || cnv == nullptr
            || uprv_strlen(name)>=ଆକାର(gDefaultConverterNameBuffer))
        {
            /* Panic time, let's use a fallback. */
#ଯଦି (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#ନଚେତଯଦି U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_SWAP_LFNL_OPTION_STRING;
#ଅନ୍ୟଥା
            name = "ibm-37_P100-1995";
#endif
        }

        internalSetName(name, &errorCode);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    ଫେରନ୍ତୁ name;
#endif
}

#ଯଦି U_CHARSET_IS_UTF8
U_CAPI ଶୂନ୍ୟ U_EXPORT2 ucnv_setDefaultName(ଅଚଳ ଅକ୍ଷର *) {}
#ଅନ୍ୟଥା
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucnv_setDefaultName(ଅଚଳ ଅକ୍ଷର *converterName) {
    ଯଦି(converterName==nullptr) {
        /* reset to the default codepage */
        gDefaultConverterName=nullptr;
    } ଅନ୍ୟଥା {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;
        ଅଚଳ ଅକ୍ଷର *name = nullptr;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &errorCode);
        ଯଦି(U_SUCCESS(errorCode) && cnv != nullptr) {
            name = ucnv_getName(cnv, &errorCode);
        }

        ଯଦି(U_SUCCESS(errorCode) && name!=nullptr) {
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

#ଯଦି !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_swap(ଅଚଳ UDataSwapper *ds,
          ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
          UErrorCode *pErrorCode) {
    ଅଚଳ UDataInfo *pInfo;
    int32_t headerSize;

    ଅଚଳ uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t offset, count, staticDataSize;
    int32_t size;

    ଅଚଳ UConverterStaticData *inStaticData;
    UConverterStaticData *outStaticData;

    ଅଚଳ _MBCSHeader *inMBCSHeader;
    _MBCSHeader *outMBCSHeader;
    _MBCSHeader mbcsHeader;
    uint32_t mbcsHeaderLength;
    UBool noFromU=ମିଥ୍ୟା;

    uint8_t outputType;

    int32_t maxFastUChar, mbcsIndexLength;

    ଅଚଳ int32_t *inExtIndexes;
    int32_t extOffset;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    /* check data format and format version */
    pInfo=(ଅଚଳ UDataInfo *)((ଅଚଳ ଅକ୍ଷର *)inData+4);
    ଯଦି(!(
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
        ଫେରନ୍ତୁ 0;
    }

    inBytes=(ଅଚଳ uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    inStaticData=(ଅଚଳ UConverterStaticData *)inBytes;
    outStaticData=(UConverterStaticData *)outBytes;

    ଯଦି(length<0) {
        staticDataSize=ds->readUInt32(inStaticData->structSize);
    } ଅନ୍ୟଥା {
        length-=headerSize;
        ଯଦି( length<(int32_t)ଆକାର(UConverterStaticData) ||
            (uint32_t)length<(staticDataSize=ds->readUInt32(inStaticData->structSize))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }
    }

    ଯଦି(length>=0) {
        /* swap the static data */
        ଯଦି(inStaticData!=outStaticData) {
            uprv_memcpy(outStaticData, inStaticData, staticDataSize);
        }

        ds->swapArray32(ds, &inStaticData->structSize, 4,
                           &outStaticData->structSize, pErrorCode);
        ds->swapArray32(ds, &inStaticData->codepage, 4,
                           &outStaticData->codepage, pErrorCode);

        ds->swapInvChars(ds, inStaticData->name, (int32_t)uprv_strlen(inStaticData->name),
                            outStaticData->name, pErrorCode);
        ଯଦି(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            ଫେରନ୍ତୁ 0;
        }
    }

    inBytes+=staticDataSize;
    ଯଦି (outBytes != nullptr) outBytes+=staticDataSize;
    ଯଦି(length>=0) {
        length-=(int32_t)staticDataSize;
    }

    /* check for supported conversionType values */
    ଯଦି(inStaticData->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        inMBCSHeader=(ଅଚଳ _MBCSHeader *)inBytes;
        outMBCSHeader=(_MBCSHeader *)outBytes;

        ଯଦି(0<=length && length<(int32_t)ଆକାର(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି(inMBCSHeader->version[0]==4 && inMBCSHeader->version[1]>=1) {
            mbcsHeaderLength=MBCS_HEADER_V4_LENGTH;
        } ଅନ୍ୟଥା ଯଦି(inMBCSHeader->version[0]==5 && inMBCSHeader->version[1]>=3 &&
                  ((mbcsHeader.options=ds->readUInt32(inMBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_MASK)==0
        ) {
            mbcsHeaderLength=mbcsHeader.options&MBCS_OPT_LENGTH_MASK;
            noFromU = (mbcsHeader.options & MBCS_OPT_NO_FROM_U) != 0;
        } ଅନ୍ୟଥା {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             inMBCSHeader->version[0], inMBCSHeader->version[1]);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ଫେରନ୍ତୁ 0;
        }

        uprv_memcpy(mbcsHeader.version, inMBCSHeader->version, 4);
        mbcsHeader.ଗଣନା_States=         ds->readUInt32(inMBCSHeader->ଗଣନା_States);
        mbcsHeader.ଗଣନା_To_UFallbacks=   ds->readUInt32(inMBCSHeader->ଗଣନା_To_UFallbacks);
        mbcsHeader.offsetToUCodeUnits=  ds->readUInt32(inMBCSHeader->offsetToUCodeUnits);
        mbcsHeader.offsetFromUTable=    ds->readUInt32(inMBCSHeader->offsetFromUTable);
        mbcsHeader.offsetFromUBytes=    ds->readUInt32(inMBCSHeader->offsetFromUBytes);
        mbcsHeader.flags=               ds->readUInt32(inMBCSHeader->flags);
        mbcsHeader.fromUBytesLength=    ds->readUInt32(inMBCSHeader->fromUBytesLength);
        /* mbcsHeader.options have been read above */

        extOffset=(int32_t)(mbcsHeader.flags>>8);
        outputType=(uint8_t)mbcsHeader.flags;
        ଯଦି(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ଫେରନ୍ତୁ 0;
        }

        /* make sure that the output type is known */
        ବାଛ(outputType) {
        ସ୍ଥିତି MBCS_OUTPUT_1:
        ସ୍ଥିତି MBCS_OUTPUT_2:
        ସ୍ଥିତି MBCS_OUTPUT_3:
        ସ୍ଥିତି MBCS_OUTPUT_4:
        ସ୍ଥିତି MBCS_OUTPUT_3_EUC:
        ସ୍ଥିତି MBCS_OUTPUT_4_EUC:
        ସ୍ଥିତି MBCS_OUTPUT_2_SISO:
        ସ୍ଥିତି MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ଫେରନ୍ତୁ 0;
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
        ଯଦି( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcsHeader.version[1]>=3 && (maxFastUChar=mbcsHeader.version[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcsIndexLength=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        ଯଦି(extOffset==0) {
            size=(int32_t)(mbcsHeader.offsetFromUBytes+mbcsIndexLength);
            ଯଦି(!noFromU) {
                size+=(int32_t)mbcsHeader.fromUBytesLength;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            inExtIndexes=nullptr;
        } ଅନ୍ୟଥା {
            /* there is extension data after the base data, see ucnv_ext.h */
            ଯଦି(length>=0 && length<(extOffset+UCNV_EXT_INDEXES_MIN_LENGTH*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ଫେରନ୍ତୁ 0;
            }

            inExtIndexes=(ଅଚଳ int32_t *)(inBytes+extOffset);
            size=extOffset+udata_readInt32(ds, inExtIndexes[UCNV_EXT_SIZE]);
        }

        ଯଦି(length>=0) {
            ଯଦି(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ଫେରନ୍ତୁ 0;
            }

            /* copy the data for inaccessible bytes */
            ଯଦି(inBytes!=outBytes) {
                uprv_memcpy(outBytes, inBytes, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcsHeaderLength*4;
            ds->swapArray32(ds, &inMBCSHeader->ଗଣନା_States, count-4,
                               &outMBCSHeader->ଗଣନା_States, pErrorCode);

            ଯଦି(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ଅଚଳ ଅକ୍ଷର *inBaseName=(ଅଚଳ ଅକ୍ଷର *)inBytes+count;
                ଅକ୍ଷର *outBaseName=(ଅକ୍ଷର *)outBytes+count;
                ds->swapInvChars(ds, inBaseName, (int32_t)uprv_strlen(inBaseName),
                                    outBaseName, pErrorCode);
            } ଅନ୍ୟଥା {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                offset=count;
                count=mbcsHeader.ଗଣନା_States*1024;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the toUFallbacks[] */
                offset+=count;
                count=mbcsHeader.ଗଣନା_To_UFallbacks*8;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the unicodeCodeUnits[] */
                offset=mbcsHeader.offsetToUCodeUnits;
                count=mbcsHeader.offsetFromUTable-offset;
                ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* offset to the stage 1 table, independent of the outputType */
                offset=mbcsHeader.offsetFromUTable;

                ଯଦି(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcsHeader.offsetFromUBytes-offset)+mbcsHeader.fromUBytesLength;
                    ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                       outBytes+offset, pErrorCode);
                } ଅନ୍ୟଥା {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    ଯଦି(inStaticData->unicodeMask&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } ଅନ୍ୟଥା {
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
                    ବାଛ(outputType) {
                    ସ୍ଥିତି MBCS_OUTPUT_2:
                    ସ୍ଥିତି MBCS_OUTPUT_3_EUC:
                    ସ୍ଥିତି MBCS_OUTPUT_2_SISO:
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        ଭାଙ୍ଗ;
                    ସ୍ଥିତି MBCS_OUTPUT_4:
                        ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        ଭାଙ୍ଗ;
                    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        /* just uint8_t[], nothing to swap */
                        ଭାଙ୍ଗ;
                    }

                    ଯଦି(mbcsIndexLength!=0) {
                        offset+=count;
                        count=mbcsIndexLength;
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                    }
                }
            }

            ଯଦି(extOffset!=0) {
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
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_ମୂଲ୍ୟଗୁଡ଼ିକ_INDEX]);
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
    } ଅନ୍ୟଥା {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         inStaticData->conversionType);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଫେରନ୍ତୁ headerSize+(int32_t)staticDataSize+size;
}

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#endif
