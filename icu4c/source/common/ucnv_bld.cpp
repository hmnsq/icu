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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "utracimp.h"
#അന്തര്ഭാവ "ucnv_io.h"
#അന്തര്ഭാവ "ucnv_bld.h"
#അന്തര്ഭാവ "ucnvmbcs.h"
#അന്തര്ഭാവ "ucnv_ext.h"
#അന്തര്ഭാവ "ucnv_cnv.h"
#അന്തര്ഭാവ "ucnv_imp.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "ustr_cnv.h"


#എങ്കിൽ 0
#അന്തര്ഭാവ <stdio.h>
ബാഹ്യ ശൂന്യം UCNV_DEBUG_LOG(അക്ഷരം *what, അക്ഷരം *who, ശൂന്യം *p, പൂർണ്ണസംഖ്യ l);
#define UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#അല്ലെങ്കിൽ
# define UCNV_DEBUG_LOG(x,y,z)
#endif

സ്ഥിതം സ്ഥിരം UConverterSharedData * സ്ഥിരം
converterData[UCNV_സംഖ്യ_OF_SUPPORTED_CONVERTER_TYPES]={
    nullptr, nullptr,

#എങ്കിൽ UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#അല്ലെങ്കിൽ
    &_MBCSData,
#endif

    &_Latin1Data,
    &_UTF8Data, &_UTF16BEData, &_UTF16LEData,
#എങ്കിൽ UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr,
#അല്ലെങ്കിൽ
    &_UTF32BEData, &_UTF32LEData,
#endif
    nullptr,

#എങ്കിൽ UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#അല്ലെങ്കിൽ
    &_ISO2022Data,
#endif

#എങ്കിൽ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr,
#അല്ലെങ്കിൽ
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#endif

#എങ്കിൽ UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#അല്ലെങ്കിൽ
    &_SCSUData,
#endif


#എങ്കിൽ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#അല്ലെങ്കിൽ
    &_ISCIIData,
#endif

    &_ASCIIData,
#എങ്കിൽ UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, &_UTF16Data, nullptr, nullptr, nullptr,
#അല്ലെങ്കിൽ
    &_UTF7Data, &_Bocu1Data, &_UTF16Data, &_UTF32Data, &_CESU8Data, &_IMAPData,
#endif

#എങ്കിൽ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#അല്ലെങ്കിൽ
    &_CompoundTextData
#endif
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
സ്ഥിതം ഘടന {
  സ്ഥിരം അക്ഷരം *name;
  സ്ഥിരം UConverterType type;
} സ്ഥിരം cnvNameType[] = {
#എങ്കിൽ !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#endif
#എങ്കിൽ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#endif
#എങ്കിൽ !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#endif
#എങ്കിൽ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#endif
#എങ്കിൽ !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#endif
  { "iso88591", UCNV_LATIN_1 },
#എങ്കിൽ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#എങ്കിൽ !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#endif
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#എങ്കിൽ U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#അല്ലെങ്കിൽ
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#endif
#എങ്കിൽ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#എങ്കിൽ U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#അല്ലെങ്കിൽ
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#endif
#endif
#എങ്കിൽ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#endif
  { "utf8", UCNV_UTF8 },
#എങ്കിൽ !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_TEXT}
#endif
};


/*initializes some global variables */
സ്ഥിതം UHashtable *SHARED_DATA_HASHTABLE = nullptr;
സ്ഥിതം icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

സ്ഥിതം സ്ഥിരം അക്ഷരം **gAvailableConverters = nullptr;
സ്ഥിതം uint16_t g_Available_Converter_എണ്ണം = 0;
സ്ഥിതം icu::UInitOnce gAvailableConvertersInitOnce {};

#എങ്കിൽ !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
സ്ഥിതം അക്ഷരം gDefaultConverterNameBuffer[UCNV_MAX_CONVERTER_NAME_LENGTH + 1]; /* +1 for nullptr */
സ്ഥിതം സ്ഥിരം അക്ഷരം *gDefaultConverterName = nullptr;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
സ്ഥിതം സ്ഥിരം UConverterSharedData *gDefaultAlgorithmicSharedData = nullptr;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
സ്ഥിതം UBool gDefaultConverterContainsOption;

#endif  /* !U_CHARSET_IS_UTF8 */

സ്ഥിതം സ്ഥിരം അക്ഷരം DATA_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
സ്ഥിതം ശൂന്യം
ucnv_flushAvailableConverterCache() {
    g_Available_Converter_എണ്ണം = 0;
    എങ്കിൽ (gAvailableConverters) {
        uprv_free(const_cast<അക്ഷരം**>(gAvailableConverters));
        gAvailableConverters = nullptr;
    }
    gAvailableConvertersInitOnce.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
സ്ഥിതം UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    എങ്കിൽ (SHARED_DATA_HASHTABLE != nullptr && uhash_എണ്ണം(SHARED_DATA_HASHTABLE) == 0) {
        uhash_close(SHARED_DATA_HASHTABLE);
        SHARED_DATA_HASHTABLE = nullptr;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flushAvailableConverterCache();

#എങ്കിൽ !U_CHARSET_IS_UTF8
    gDefaultConverterName = nullptr;
    gDefaultConverterNameBuffer[0] = 0;
    gDefaultConverterContainsOption = അസത്യം;
    gDefaultAlgorithmicSharedData = nullptr;
#endif

    തിരികെ (SHARED_DATA_HASHTABLE == nullptr);
}

U_CAPI ശൂന്യം U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_registerCleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

സ്ഥിതം UBool U_CALLCONV
isCnvAcceptable(ശൂന്യം * /*context*/,
                സ്ഥിരം അക്ഷരം * /*type*/, സ്ഥിരം അക്ഷരം * /*name*/,
                സ്ഥിരം UDataInfo *pInfo) {
    തിരികെ
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
സ്ഥിതം UConverterSharedData*
ucnv_data_unFlattenClone(UConverterLoadArgs *pArgs, UDataMemory *pData, UErrorCode *status)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    സ്ഥിരം uint8_t* raw = static_cast<സ്ഥിരം uint8_t*>(udata_getMemory(pData));
    സ്ഥിരം UConverterStaticData* source = reinterpret_cast<സ്ഥിരം UConverterStaticData*>(raw);
    UConverterSharedData *data;
    UConverterType type = static_cast<UConverterType>(source->conversionType);

    എങ്കിൽ(U_FAILURE(*status))
        തിരികെ nullptr;

    എങ്കിൽ (static_cast<uint16_t>(type) >= UCNV_സംഖ്യ_OF_SUPPORTED_CONVERTER_TYPES ||
        converterData[type] == nullptr ||
        !converterData[type]->isReferenceCounted ||
        converterData[type]->referenceCounter != 1 ||
        source->structSize != വലുപ്പം(UConverterStaticData))
    {
        *status = U_INVALID_TABLE_FORMAT;
        തിരികെ nullptr;
    }

    data = static_cast<UConverterSharedData*>(uprv_malloc(വലുപ്പം(UConverterSharedData)));
    എങ്കിൽ(data == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(data, converterData[type], വലുപ്പം(UConverterSharedData));

    data->staticData = source;

    data->sharedDataCached = അസത്യം;

    /* fill in fields from the loaded data */
    data->dataMemory = (ശൂന്യം*)pData; /* for future use */

    എങ്കിൽ(data->impl->load != nullptr) {
        data->impl->load(data, pArgs, raw + source->structSize, status);
        എങ്കിൽ(U_FAILURE(*status)) {
            uprv_free(data);
            തിരികെ nullptr;
        }
    }
    തിരികെ data;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
സ്ഥിതം UConverterSharedData *createConverterFromFile(UConverterLoadArgs *pArgs, UErrorCode * err)
{
    UDataMemory *data;
    UConverterSharedData *sharedData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_LOAD);

    എങ്കിൽ (U_FAILURE (*err)) {
        UTRACE_EXIT_STATUS(*err);
        തിരികെ nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", pArgs->name, pArgs->pkg);

    data = udata_openChoice(pArgs->pkg, DATA_TYPE, pArgs->name, isCnvAcceptable, nullptr, err);
    എങ്കിൽ(U_FAILURE(*err))
    {
        UTRACE_EXIT_STATUS(*err);
        തിരികെ nullptr;
    }

    sharedData = ucnv_data_unFlattenClone(pArgs, data, err);
    എങ്കിൽ(U_FAILURE(*err))
    {
        udata_close(data);
        UTRACE_EXIT_STATUS(*err);
        തിരികെ nullptr;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_STATUS(sharedData, *err);
    തിരികെ sharedData;
}

/*returns a converter type from a string
 */
സ്ഥിതം സ്ഥിരം UConverterSharedData *
getAlgorithmicTypeFromName(സ്ഥിരം അക്ഷരം *realName)
{
    uint32_t mid, start, limit;
    uint32_t lastMid;
    പൂർണ്ണസംഖ്യ result;
    അക്ഷരം strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    start = 0;
    limit = UPRV_LENGTHOF(cnvNameType);
    mid = limit;
    lastMid = UINT32_MAX;

    വേണ്ടി (;;) {
        mid = (start + limit) / 2;
        എങ്കിൽ (lastMid == mid) {   /* Have we moved? */
            നിർത്തുക;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        result = uprv_strcmp(strippedName, cnvNameType[mid].name);

        എങ്കിൽ (result < 0) {
            limit = mid;
        } അല്ലെങ്കിൽ എങ്കിൽ (result > 0) {
            start = mid;
        } അല്ലെങ്കിൽ {
            തിരികെ converterData[cnvNameType[mid].type];
        }
    }

    തിരികെ nullptr;
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
സ്ഥിതം ശൂന്യം
ucnv_shareConverterData(UConverterSharedData * data)
{
    UErrorCode err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    എങ്കിൽ (SHARED_DATA_HASHTABLE == nullptr)
    {
        SHARED_DATA_HASHTABLE = uhash_openSize(uhash_hashChars, uhash_compareChars, nullptr,
                            ucnv_io_എണ്ണം_Known_Converters(&err)*UCNV_CACHE_LOAD_FACTOR,
                            &err);
        ucnv_enableCleanup();

        എങ്കിൽ (U_FAILURE(err))
            തിരികെ;
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
    data->sharedDataCached = സത്യം;

    uhash_put(SHARED_DATA_HASHTABLE,
            (ശൂന്യം*) data->staticData->name, /* Okay to cast away const as long as
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
സ്ഥിതം UConverterSharedData *
ucnv_getSharedConverterData(സ്ഥിരം അക്ഷരം *name)
{
    /*special case when no Table has yet been created we return nullptr */
    എങ്കിൽ (SHARED_DATA_HASHTABLE == nullptr)
    {
        തിരികെ nullptr;
    }
    അല്ലെങ്കിൽ
    {
        UConverterSharedData *rc;

        rc = static_cast<UConverterSharedData*>(uhash_get(SHARED_DATA_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        തിരികെ rc;
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
സ്ഥിതം UBool
ucnv_deleteSharedConverterData(UConverterSharedData * deadSharedData)
{
    UTRACE_ENTRY_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", deadSharedData->staticData->name, deadSharedData);

    എങ്കിൽ (deadSharedData->referenceCounter > 0) {
        UTRACE_EXIT_മൂല്യം((int32_t)അസത്യം);
        തിരികെ അസത്യം;
    }

    എങ്കിൽ (deadSharedData->impl->unload != nullptr) {
        deadSharedData->impl->unload(deadSharedData);
    }

    എങ്കിൽ(deadSharedData->dataMemory != nullptr)
    {
        UDataMemory *data = (UDataMemory*)deadSharedData->dataMemory;
        udata_close(data);
    }

    uprv_free(deadSharedData);

    UTRACE_EXIT_മൂല്യം((int32_t)സത്യം);
    തിരികെ സത്യം;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverterSharedData *
ucnv_load(UConverterLoadArgs *pArgs, UErrorCode *err) {
    UConverterSharedData *mySharedConverterData;

    എങ്കിൽ(err == nullptr || U_FAILURE(*err)) {
        തിരികെ nullptr;
    }

    എങ്കിൽ(pArgs->pkg != nullptr && *pArgs->pkg != 0) {
        /* application-provided converters are not currently cached */
        തിരികെ createConverterFromFile(pArgs, err);
    }

    mySharedConverterData = ucnv_getSharedConverterData(pArgs->name);
    എങ്കിൽ (mySharedConverterData == nullptr)
    {
        /*Not cached, we need to stream it in from file */
        mySharedConverterData = createConverterFromFile(pArgs, err);
        എങ്കിൽ (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            തിരികെ nullptr;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (!pArgs->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_shareConverterData(mySharedConverterData);
        }
    }
    അല്ലെങ്കിൽ
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        mySharedConverterData->referenceCounter++;
    }

    തിരികെ mySharedConverterData;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI ശൂന്യം
ucnv_unload(UConverterSharedData *sharedData) {
    എങ്കിൽ(sharedData != nullptr) {
        എങ്കിൽ (sharedData->referenceCounter > 0) {
            sharedData->referenceCounter--;
        }

        എങ്കിൽ((sharedData->referenceCounter <= 0)&&(sharedData->sharedDataCached == അസത്യം)) {
            ucnv_deleteSharedConverterData(sharedData);
        }
    }
}

U_CFUNC ശൂന്യം
ucnv_unloadSharedDataIfReady(UConverterSharedData *sharedData)
{
    എങ്കിൽ(sharedData != nullptr && sharedData->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        ucnv_unload(sharedData);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC ശൂന്യം
ucnv_increment_Ref_എണ്ണം(UConverterSharedData *sharedData)
{
    എങ്കിൽ(sharedData != nullptr && sharedData->isReferenceCounted) {
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
സ്ഥിതം ശൂന്യം
parseConverterOptions(സ്ഥിരം അക്ഷരം *inName,
                      UConverterNamePieces *pPieces,
                      UConverterLoadArgs *pArgs,
                      UErrorCode *err)
{
    അക്ഷരം *cnvName = pPieces->cnvName;
    അക്ഷരം c;
    int32_t len = 0;

    pArgs->name=inName;
    pArgs->locale=pPieces->locale;
    pArgs->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    ആകുമ്പോൾ((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        എങ്കിൽ (++len>=UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *err = U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            തിരികെ;
        }
        *cnvName++=c;
        inName++;
    }
    *cnvName=0;
    pArgs->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    ആകുമ്പോൾ((c=*inName)!=0) {
        എങ്കിൽ(c==UCNV_OPTION_SEP_CHAR) {
            ++inName;
        }

        /* inName is behind an option separator */
        എങ്കിൽ(uprv_strncmp(inName, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            അക്ഷരം *dest=pPieces->locale;

            /* copy the locale option value */
            inName+=7;
            len=0;
            ആകുമ്പോൾ((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++inName;

                എങ്കിൽ(++len>=ULOC_FULLNAME_CAPACITY) {
                    *err=U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    തിരികെ;
                }

                *dest++=c;
            }
            *dest=0;
        } അല്ലെങ്കിൽ എങ്കിൽ(uprv_strncmp(inName, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            inName+=8;
            c=*inName;
            എങ്കിൽ(c==0) {
                pArgs->options=(pPieces->options&=~UCNV_OPTION_VERSION);
                തിരികെ;
            } അല്ലെങ്കിൽ എങ്കിൽ (static_cast<uint8_t>(c - '0') < 10) {
                pArgs->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_VERSION) | static_cast<uint32_t>(c - '0');
                ++inName;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(uprv_strncmp(inName, "swaplfnl", 8)==0) {
            inName+=8;
            pArgs->options=(pPieces->options|=UCNV_OPTION_SWAP_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } അല്ലെങ്കിൽ {
            /* ignore any other options until we define some */
            ആകുമ്പോൾ(((c = *inName++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            എങ്കിൽ(c==0) {
                തിരികെ;
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
ucnv_loadSharedData(സ്ഥിരം അക്ഷരം *converterName,
                    UConverterNamePieces *pPieces,
                    UConverterLoadArgs *pArgs,
                    UErrorCode * err) {
    UConverterNamePieces അട്ടി_Pieces;
    UConverterLoadArgs അട്ടി_Args;
    UConverterSharedData *mySharedConverterData = nullptr;
    UErrorCode internalErrorCode = U_ZERO_ERROR;
    UBool mayContainOption = സത്യം;
    UBool checkForAlgorithmic = സത്യം;

    എങ്കിൽ (U_FAILURE (*err)) {
        തിരികെ nullptr;
    }

    എങ്കിൽ(pPieces == nullptr) {
        എങ്കിൽ(pArgs != nullptr) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_INTERNAL_PROGRAM_ERROR;
            തിരികെ nullptr;
        }
        pPieces = &അട്ടി_Pieces;
    }
    എങ്കിൽ(pArgs == nullptr) {
        uprv_memset(&അട്ടി_Args, 0, വലുപ്പം(അട്ടി_Args));
        അട്ടി_Args.size = (int32_t)വലുപ്പം(അട്ടി_Args);
        pArgs = &അട്ടി_Args;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    pArgs->name = converterName;
    pArgs->locale = pPieces->locale;
    pArgs->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    എങ്കിൽ (converterName == nullptr) {
#എങ്കിൽ U_CHARSET_IS_UTF8
        pArgs->name = "UTF-8";
        തിരികെ (UConverterSharedData *)converterData[UCNV_UTF8];
#അല്ലെങ്കിൽ
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        pArgs->name = ucnv_getDefaultName();
        എങ്കിൽ (pArgs->name == nullptr) {
            *err = U_MISSING_RESOURCE_ERROR;
            തിരികെ nullptr;
        }
        mySharedConverterData = (UConverterSharedData *)gDefaultAlgorithmicSharedData;
        checkForAlgorithmic = അസത്യം;
        mayContainOption = gDefaultConverterContainsOption;
        /* the default converter name is already canonical */
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        pArgs->name = "UTF-8";
        തിരികെ (UConverterSharedData *)converterData[UCNV_UTF8];
    }
    അല്ലെങ്കിൽ {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, pArgs, err);
        എങ്കിൽ (U_FAILURE(*err)) {
            /* Very bad name used. */
            തിരികെ nullptr;
        }

        /* get the canonical converter name */
        pArgs->name = ucnv_io_getConverterName(pArgs->name, &mayContainOption, &internalErrorCode);
        എങ്കിൽ (U_FAILURE(internalErrorCode) || pArgs->name == nullptr) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            pArgs->name = pPieces->cnvName;
        } അല്ലെങ്കിൽ എങ്കിൽ (internalErrorCode == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    എങ്കിൽ(mayContainOption && pArgs->name != pPieces->cnvName) {
        parseConverterOptions(pArgs->name, pPieces, pArgs, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    എങ്കിൽ (checkForAlgorithmic) {
        mySharedConverterData = (UConverterSharedData *)getAlgorithmicTypeFromName(pArgs->name);
    }
    എങ്കിൽ (mySharedConverterData == nullptr)
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
        എങ്കിൽ (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            തിരികെ nullptr;
        }
    }

    തിരികെ mySharedConverterData;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     സ്ഥിരം അക്ഷരം* converterName,
                     UErrorCode* err)
{
    UConverterNamePieces അട്ടി_Pieces;
    UConverterLoadArgs അട്ടി_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    എങ്കിൽ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        mySharedConverterData = ucnv_loadSharedData(converterName, &അട്ടി_Pieces, &അട്ടി_Args, err);

        myUConverter = ucnv_createConverterFromSharedData(
            myUConverter, mySharedConverterData,
            &അട്ടി_Args,
            err);

        എങ്കിൽ(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
            തിരികെ myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_STATUS(*err);
    തിരികെ nullptr;
}

U_CFUNC UBool
ucnv_canCreateConverter(സ്ഥിരം അക്ഷരം *converterName, UErrorCode *err) {
    UConverter myUConverter;
    UConverterNamePieces അട്ടി_Pieces;
    UConverterLoadArgs അട്ടി_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    എങ്കിൽ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        അട്ടി_Args.onlyTestIsLoadable=സത്യം;
        mySharedConverterData = ucnv_loadSharedData(converterName, &അട്ടി_Pieces, &അട്ടി_Args, err);
        ucnv_createConverterFromSharedData(
            &myUConverter, mySharedConverterData,
            &അട്ടി_Args,
            err);
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
    }

    UTRACE_EXIT_STATUS(*err);
    തിരികെ U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                സ്ഥിരം അക്ഷരം* locale,
                                uint32_t options,
                                UErrorCode* err) {
    UConverter *cnv;
    സ്ഥിരം UConverterSharedData *sharedData;
    UConverterLoadArgs അട്ടി_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    എങ്കിൽ(type<0 || UCNV_സംഖ്യ_OF_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        തിരികെ nullptr;
    }

    sharedData = converterData[type];
    എങ്കിൽ(sharedData == nullptr || sharedData->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        തിരികെ nullptr;
    }

    അട്ടി_Args.name = "";
    അട്ടി_Args.options = options;
    അട്ടി_Args.locale=locale;
    cnv = ucnv_createConverterFromSharedData(
            myUConverter, const_cast<UConverterSharedData*>(sharedData),
            &അട്ടി_Args, err);

    UTRACE_EXIT_PTR_STATUS(cnv, *err);
    തിരികെ cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(സ്ഥിരം അക്ഷരം *packageName, സ്ഥിരം അക്ഷരം *converterName, UErrorCode * err)
{
    UConverter *myUConverter;
    UConverterSharedData *mySharedConverterData;
    UConverterNamePieces അട്ടി_Pieces;
    UConverterLoadArgs അട്ടി_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_PACKAGE);

    എങ്കിൽ(U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        തിരികെ nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    അട്ടി_Pieces.cnvName[0] = 0;
    അട്ടി_Pieces.locale[0] = 0;
    അട്ടി_Pieces.options = 0;
    parseConverterOptions(converterName, &അട്ടി_Pieces, &അട്ടി_Args, err);
    എങ്കിൽ (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_STATUS(*err);
        തിരികെ nullptr;
    }
    അട്ടി_Args.nestedLoads=1;
    അട്ടി_Args.pkg=packageName;

    /* open the data, unflatten the shared structure */
    mySharedConverterData = createConverterFromFile(&അട്ടി_Args, err);

    എങ്കിൽ (U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        തിരികെ nullptr;
    }

    /* create the actual converter */
    myUConverter = ucnv_createConverterFromSharedData(nullptr, mySharedConverterData, &അട്ടി_Args, err);

    എങ്കിൽ (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_STATUS(*err);
        തിരികെ nullptr;
    }

    UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
    തിരികെ myUConverter;
}


U_CFUNC UConverter*
ucnv_createConverterFromSharedData(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverterSharedData* mySharedConverterData,
                                   UConverterLoadArgs* pArgs,
                                   UErrorCode* err)
{
    UBool isCopyLocal;

    എങ്കിൽ(U_FAILURE(*err)) {
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
        തിരികെ myUConverter;
    }
    എങ്കിൽ(myUConverter == nullptr)
    {
        myUConverter = (UConverter *) uprv_malloc (വലുപ്പം (UConverter));
        എങ്കിൽ(myUConverter == nullptr)
        {
            *err = U_MEMORY_ALLOCATION_ERROR;
            ucnv_unloadSharedDataIfReady(mySharedConverterData);
            തിരികെ nullptr;
        }
        isCopyLocal = അസത്യം;
    } അല്ലെങ്കിൽ {
        isCopyLocal = സത്യം;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, വലുപ്പം(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->sharedData = mySharedConverterData;
    myUConverter->options = pArgs->options;
    എങ്കിൽ(!pArgs->onlyTestIsLoadable) {
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

    എങ്കിൽ(mySharedConverterData->impl->open != nullptr) {
        mySharedConverterData->impl->open(myUConverter, pArgs, err);
        എങ്കിൽ(U_FAILURE(*err) && !pArgs->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            തിരികെ nullptr;
        }
    }

    തിരികെ myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverterSharedData *mySharedData = nullptr;
    int32_t pos;
    int32_t table_Deleted_അക്കം = 0;
    സ്ഥിരം UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, remaining;

    UTRACE_ENTRY_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    എങ്കിൽ (SHARED_DATA_HASHTABLE == nullptr) {
        UTRACE_EXIT_മൂല്യം((int32_t)0);
        തിരികെ 0;
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
    ചെയ്യുക {
        remaining = 0;
        pos = UHASH_FIRST;
        ആകുമ്പോൾ ((e = uhash_nextElement (SHARED_DATA_HASHTABLE, &pos)) != nullptr)
        {
            mySharedData = (UConverterSharedData *) e->value.pointer;
            /*deletes only if reference counter == 0 */
            എങ്കിൽ (mySharedData->referenceCounter == 0)
            {
                table_Deleted_അക്കം++;

                UCNV_DEBUG_LOG("del",mySharedData->staticData->name,mySharedData);

                uhash_removeElement(SHARED_DATA_HASHTABLE, e);
                mySharedData->sharedDataCached = അസത്യം;
                ucnv_deleteSharedConverterData (mySharedData);
            } അല്ലെങ്കിൽ {
                ++remaining;
            }
        }
    } ആകുമ്പോൾ(++i == 1 && remaining > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", remaining);

    UTRACE_EXIT_മൂല്യം(table_Deleted_അക്കം);
    തിരികെ table_Deleted_അക്കം;
}

/* available converters list --------------------------------------------------- */

സ്ഥിതം ശൂന്യം U_CALLCONV initAvailableConvertersList(UErrorCode &errCode) {
    U_ASSERT(g_Available_Converter_എണ്ണം == 0);
    U_ASSERT(gAvailableConverters == nullptr);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_openAllNames(&errCode);
    int32_t all_Converter_എണ്ണം = uenum_എണ്ണം(allConvEnum, &errCode);
    എങ്കിൽ (U_FAILURE(errCode)) {
        തിരികെ;
    }

    /* We can't have more than "*converterTable" converters to open */
    gAvailableConverters = static_cast<സ്ഥിരം അക്ഷരം**>(uprv_malloc(all_Converter_എണ്ണം * വലുപ്പം(അക്ഷരം*)));
    എങ്കിൽ (!gAvailableConverters) {
        errCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UErrorCode localStatus = U_ZERO_ERROR;
    UConverter tempConverter;
    ucnv_close(ucnv_createConverter(&tempConverter, nullptr, &localStatus));

    g_Available_Converter_എണ്ണം = 0;

    വേണ്ടി (int32_t idx = 0; idx < all_Converter_എണ്ണം; idx++) {
        localStatus = U_ZERO_ERROR;
        സ്ഥിരം അക്ഷരം *converterName = uenum_next(allConvEnum, nullptr, &localStatus);
        എങ്കിൽ (ucnv_canCreateConverter(converterName, &localStatus)) {
            gAvailableConverters[g_Available_Converter_എണ്ണം++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


സ്ഥിതം UBool haveAvailableConverterList(UErrorCode *pErrorCode) {
    umtx_initOnce(gAvailableConvertersInitOnce, &initAvailableConvertersList, *pErrorCode);
    തിരികെ U_SUCCESS(*pErrorCode);
}

U_CFUNC uint16_t
ucnv_bld_എണ്ണം_Available_Converters(UErrorCode *pErrorCode) {
    എങ്കിൽ (haveAvailableConverterList(pErrorCode)) {
        തിരികെ g_Available_Converter_എണ്ണം;
    }
    തിരികെ 0;
}

U_CFUNC സ്ഥിരം അക്ഷരം *
ucnv_bld_getAvailableConverter(uint16_t n, UErrorCode *pErrorCode) {
    എങ്കിൽ (haveAvailableConverterList(pErrorCode)) {
        എങ്കിൽ (n < g_Available_Converter_എണ്ണം) {
            തിരികെ gAvailableConverters[n];
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }
    തിരികെ nullptr;
}

/* default converter name --------------------------------------------------- */

#എങ്കിൽ !U_CHARSET_IS_UTF8
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
സ്ഥിതം വരിയിൽ ശൂന്യം
internalSetName(സ്ഥിരം അക്ഷരം *name, UErrorCode *status) {
    UConverterNamePieces അട്ടി_Pieces;
    UConverterLoadArgs അട്ടി_Args=UCNV_LOAD_ARGS_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool containsOption = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != nullptr);
    സ്ഥിരം UConverterSharedData *algorithmicSharedData;

    അട്ടി_Args.name = name;
    എങ്കിൽ(containsOption) {
        അട്ടി_Pieces.cnvName[0] = 0;
        അട്ടി_Pieces.locale[0] = 0;
        അട്ടി_Pieces.options = 0;
        parseConverterOptions(name, &അട്ടി_Pieces, &അട്ടി_Args, status);
        എങ്കിൽ(U_FAILURE(*status)) {
            തിരികെ;
        }
    }
    algorithmicSharedData = getAlgorithmicTypeFromName(അട്ടി_Args.name);

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

U_CAPI സ്ഥിരം അക്ഷരം*  U_EXPORT2
ucnv_getDefaultName() {
#എങ്കിൽ U_CHARSET_IS_UTF8
    തിരികെ "UTF-8";
#അല്ലെങ്കിൽ
    /* local variable to be thread-safe */
    സ്ഥിരം അക്ഷരം *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex lock(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    എങ്കിൽ(name==nullptr) {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        എങ്കിൽ(name != nullptr) {
            cnv = ucnv_open(name, &errorCode);
            എങ്കിൽ(U_SUCCESS(errorCode) && cnv != nullptr) {
                name = ucnv_getName(cnv, &errorCode);
            }
        }

        എങ്കിൽ(name == nullptr || name[0] == 0
            || U_FAILURE(errorCode) || cnv == nullptr
            || uprv_strlen(name)>=വലുപ്പം(gDefaultConverterNameBuffer))
        {
            /* Panic time, let's use a fallback. */
#എങ്കിൽ (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_SWAP_LFNL_OPTION_STRING;
#അല്ലെങ്കിൽ
            name = "ibm-37_P100-1995";
#endif
        }

        internalSetName(name, &errorCode);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    തിരികെ name;
#endif
}

#എങ്കിൽ U_CHARSET_IS_UTF8
U_CAPI ശൂന്യം U_EXPORT2 ucnv_setDefaultName(സ്ഥിരം അക്ഷരം *) {}
#അല്ലെങ്കിൽ
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI ശൂന്യം U_EXPORT2
ucnv_setDefaultName(സ്ഥിരം അക്ഷരം *converterName) {
    എങ്കിൽ(converterName==nullptr) {
        /* reset to the default codepage */
        gDefaultConverterName=nullptr;
    } അല്ലെങ്കിൽ {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;
        സ്ഥിരം അക്ഷരം *name = nullptr;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &errorCode);
        എങ്കിൽ(U_SUCCESS(errorCode) && cnv != nullptr) {
            name = ucnv_getName(cnv, &errorCode);
        }

        എങ്കിൽ(U_SUCCESS(errorCode) && name!=nullptr) {
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

#എങ്കിൽ !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_swap(സ്ഥിരം UDataSwapper *ds,
          സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
          UErrorCode *pErrorCode) {
    സ്ഥിരം UDataInfo *pInfo;
    int32_t headerSize;

    സ്ഥിരം uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t offset, count, staticDataSize;
    int32_t size;

    സ്ഥിരം UConverterStaticData *inStaticData;
    UConverterStaticData *outStaticData;

    സ്ഥിരം _MBCSHeader *inMBCSHeader;
    _MBCSHeader *outMBCSHeader;
    _MBCSHeader mbcsHeader;
    uint32_t mbcsHeaderLength;
    UBool noFromU=അസത്യം;

    uint8_t outputType;

    int32_t maxFastUChar, mbcsIndexLength;

    സ്ഥിരം int32_t *inExtIndexes;
    int32_t extOffset;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    /* check data format and format version */
    pInfo=(സ്ഥിരം UDataInfo *)((സ്ഥിരം അക്ഷരം *)inData+4);
    എങ്കിൽ(!(
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
        തിരികെ 0;
    }

    inBytes=(സ്ഥിരം uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    inStaticData=(സ്ഥിരം UConverterStaticData *)inBytes;
    outStaticData=(UConverterStaticData *)outBytes;

    എങ്കിൽ(length<0) {
        staticDataSize=ds->readUInt32(inStaticData->structSize);
    } അല്ലെങ്കിൽ {
        length-=headerSize;
        എങ്കിൽ( length<(int32_t)വലുപ്പം(UConverterStaticData) ||
            (uint32_t)length<(staticDataSize=ds->readUInt32(inStaticData->structSize))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
    }

    എങ്കിൽ(length>=0) {
        /* swap the static data */
        എങ്കിൽ(inStaticData!=outStaticData) {
            uprv_memcpy(outStaticData, inStaticData, staticDataSize);
        }

        ds->swapArray32(ds, &inStaticData->structSize, 4,
                           &outStaticData->structSize, pErrorCode);
        ds->swapArray32(ds, &inStaticData->codepage, 4,
                           &outStaticData->codepage, pErrorCode);

        ds->swapInvChars(ds, inStaticData->name, (int32_t)uprv_strlen(inStaticData->name),
                            outStaticData->name, pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            തിരികെ 0;
        }
    }

    inBytes+=staticDataSize;
    എങ്കിൽ (outBytes != nullptr) outBytes+=staticDataSize;
    എങ്കിൽ(length>=0) {
        length-=(int32_t)staticDataSize;
    }

    /* check for supported conversionType values */
    എങ്കിൽ(inStaticData->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        inMBCSHeader=(സ്ഥിരം _MBCSHeader *)inBytes;
        outMBCSHeader=(_MBCSHeader *)outBytes;

        എങ്കിൽ(0<=length && length<(int32_t)വലുപ്പം(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
        എങ്കിൽ(inMBCSHeader->version[0]==4 && inMBCSHeader->version[1]>=1) {
            mbcsHeaderLength=MBCS_HEADER_V4_LENGTH;
        } അല്ലെങ്കിൽ എങ്കിൽ(inMBCSHeader->version[0]==5 && inMBCSHeader->version[1]>=3 &&
                  ((mbcsHeader.options=ds->readUInt32(inMBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_MASK)==0
        ) {
            mbcsHeaderLength=mbcsHeader.options&MBCS_OPT_LENGTH_MASK;
            noFromU = (mbcsHeader.options & MBCS_OPT_NO_FROM_U) != 0;
        } അല്ലെങ്കിൽ {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             inMBCSHeader->version[0], inMBCSHeader->version[1]);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            തിരികെ 0;
        }

        uprv_memcpy(mbcsHeader.version, inMBCSHeader->version, 4);
        mbcsHeader.എണ്ണം_States=         ds->readUInt32(inMBCSHeader->എണ്ണം_States);
        mbcsHeader.എണ്ണം_To_UFallbacks=   ds->readUInt32(inMBCSHeader->എണ്ണം_To_UFallbacks);
        mbcsHeader.offsetToUCodeUnits=  ds->readUInt32(inMBCSHeader->offsetToUCodeUnits);
        mbcsHeader.offsetFromUTable=    ds->readUInt32(inMBCSHeader->offsetFromUTable);
        mbcsHeader.offsetFromUBytes=    ds->readUInt32(inMBCSHeader->offsetFromUBytes);
        mbcsHeader.flags=               ds->readUInt32(inMBCSHeader->flags);
        mbcsHeader.fromUBytesLength=    ds->readUInt32(inMBCSHeader->fromUBytesLength);
        /* mbcsHeader.options have been read above */

        extOffset=(int32_t)(mbcsHeader.flags>>8);
        outputType=(uint8_t)mbcsHeader.flags;
        എങ്കിൽ(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *pErrorCode=U_UNSUPPORTED_ERROR;
            തിരികെ 0;
        }

        /* make sure that the output type is known */
        തിരഞ്ഞെടുപ്പ്(outputType) {
        അവസ്ഥ MBCS_OUTPUT_1:
        അവസ്ഥ MBCS_OUTPUT_2:
        അവസ്ഥ MBCS_OUTPUT_3:
        അവസ്ഥ MBCS_OUTPUT_4:
        അവസ്ഥ MBCS_OUTPUT_3_EUC:
        അവസ്ഥ MBCS_OUTPUT_4_EUC:
        അവസ്ഥ MBCS_OUTPUT_2_SISO:
        അവസ്ഥ MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            തിരികെ 0;
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
        എങ്കിൽ( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcsHeader.version[1]>=3 && (maxFastUChar=mbcsHeader.version[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcsIndexLength=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        എങ്കിൽ(extOffset==0) {
            size=(int32_t)(mbcsHeader.offsetFromUBytes+mbcsIndexLength);
            എങ്കിൽ(!noFromU) {
                size+=(int32_t)mbcsHeader.fromUBytesLength;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            inExtIndexes=nullptr;
        } അല്ലെങ്കിൽ {
            /* there is extension data after the base data, see ucnv_ext.h */
            എങ്കിൽ(length>=0 && length<(extOffset+UCNV_EXT_INDEXES_MIN_LENGTH*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                തിരികെ 0;
            }

            inExtIndexes=(സ്ഥിരം int32_t *)(inBytes+extOffset);
            size=extOffset+udata_readInt32(ds, inExtIndexes[UCNV_EXT_SIZE]);
        }

        എങ്കിൽ(length>=0) {
            എങ്കിൽ(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                തിരികെ 0;
            }

            /* copy the data for inaccessible bytes */
            എങ്കിൽ(inBytes!=outBytes) {
                uprv_memcpy(outBytes, inBytes, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcsHeaderLength*4;
            ds->swapArray32(ds, &inMBCSHeader->എണ്ണം_States, count-4,
                               &outMBCSHeader->എണ്ണം_States, pErrorCode);

            എങ്കിൽ(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                സ്ഥിരം അക്ഷരം *inBaseName=(സ്ഥിരം അക്ഷരം *)inBytes+count;
                അക്ഷരം *outBaseName=(അക്ഷരം *)outBytes+count;
                ds->swapInvChars(ds, inBaseName, (int32_t)uprv_strlen(inBaseName),
                                    outBaseName, pErrorCode);
            } അല്ലെങ്കിൽ {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                offset=count;
                count=mbcsHeader.എണ്ണം_States*1024;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the toUFallbacks[] */
                offset+=count;
                count=mbcsHeader.എണ്ണം_To_UFallbacks*8;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the unicodeCodeUnits[] */
                offset=mbcsHeader.offsetToUCodeUnits;
                count=mbcsHeader.offsetFromUTable-offset;
                ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* offset to the stage 1 table, independent of the outputType */
                offset=mbcsHeader.offsetFromUTable;

                എങ്കിൽ(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcsHeader.offsetFromUBytes-offset)+mbcsHeader.fromUBytesLength;
                    ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                       outBytes+offset, pErrorCode);
                } അല്ലെങ്കിൽ {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    എങ്കിൽ(inStaticData->unicodeMask&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } അല്ലെങ്കിൽ {
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
                    തിരഞ്ഞെടുപ്പ്(outputType) {
                    അവസ്ഥ MBCS_OUTPUT_2:
                    അവസ്ഥ MBCS_OUTPUT_3_EUC:
                    അവസ്ഥ MBCS_OUTPUT_2_SISO:
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        നിർത്തുക;
                    അവസ്ഥ MBCS_OUTPUT_4:
                        ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        നിർത്തുക;
                    സ്ഥിരസ്ഥിതി:
                        /* just uint8_t[], nothing to swap */
                        നിർത്തുക;
                    }

                    എങ്കിൽ(mbcsIndexLength!=0) {
                        offset+=count;
                        count=mbcsIndexLength;
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                    }
                }
            }

            എങ്കിൽ(extOffset!=0) {
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
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_മൂല്യങ്ങൾ_INDEX]);
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
    } അല്ലെങ്കിൽ {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         inStaticData->conversionType);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        തിരികെ 0;
    }

    തിരികെ headerSize+(int32_t)staticDataSize+size;
}

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#endif
