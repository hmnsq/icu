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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "unicode/ucnv.h"
#অন্তর্ভাৱ "unicode/uloc.h"
#অন্তর্ভাৱ "mutex.h"
#অন্তর্ভাৱ "putilimp.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "utracimp.h"
#অন্তর্ভাৱ "ucnv_io.h"
#অন্তর্ভাৱ "ucnv_bld.h"
#অন্তর্ভাৱ "ucnvmbcs.h"
#অন্তর্ভাৱ "ucnv_ext.h"
#অন্তর্ভাৱ "ucnv_cnv.h"
#অন্তর্ভাৱ "ucnv_imp.h"
#অন্তর্ভাৱ "uhash.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "ucln_cmn.h"
#অন্তর্ভাৱ "ustr_cnv.h"


#যদি 0
#অন্তর্ভাৱ <stdio.h>
বাহ্য শূন্য UCNV_DEBUG_LOG(ৱর্ণ *what, ৱর্ণ *who, শূন্য *p, পূর্ণাঙ্ক l);
#define UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#অন্যথা
# define UCNV_DEBUG_LOG(x,y,z)
#endif

স্থির ধ্রুৱ UConverterSharedData * ধ্রুৱ
converterData[UCNV_সংখ্যা_OF_SUPPORTED_CONVERTER_TYPES]={
    nullptr, nullptr,

#যদি UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#অন্যথা
    &_MBCSData,
#endif

    &_Latin1Data,
    &_UTF8Data, &_UTF16BEData, &_UTF16LEData,
#যদি UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr,
#অন্যথা
    &_UTF32BEData, &_UTF32LEData,
#endif
    nullptr,

#যদি UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#অন্যথা
    &_ISO2022Data,
#endif

#যদি UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr,
#অন্যথা
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#endif

#যদি UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#অন্যথা
    &_SCSUData,
#endif


#যদি UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#অন্যথা
    &_ISCIIData,
#endif

    &_ASCIIData,
#যদি UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, &_UTF16Data, nullptr, nullptr, nullptr,
#অন্যথা
    &_UTF7Data, &_Bocu1Data, &_UTF16Data, &_UTF32Data, &_CESU8Data, &_IMAPData,
#endif

#যদি UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#অন্যথা
    &_CompoundTextData
#endif
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
স্থির সংরচনা {
  ধ্রুৱ ৱর্ণ *name;
  ধ্রুৱ UConverterType type;
} ধ্রুৱ cnvNameType[] = {
#যদি !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#endif
#যদি !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#endif
#যদি !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#endif
#যদি !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#endif
#যদি !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#endif
  { "iso88591", UCNV_LATIN_1 },
#যদি !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#যদি !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#endif
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#যদি U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#অন্যথা
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#endif
#যদি !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#যদি U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#অন্যথা
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#endif
#endif
#যদি !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#endif
  { "utf8", UCNV_UTF8 },
#যদি !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_TEXT}
#endif
};


/*initializes some global variables */
স্থির UHashtable *SHARED_DATA_HASHTABLE = nullptr;
স্থির icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

স্থির ধ্রুৱ ৱর্ণ **gAvailableConverters = nullptr;
স্থির uint16_t g_Available_Converter_গুনতি = 0;
স্থির icu::UInitOnce gAvailableConvertersInitOnce {};

#যদি !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
স্থির ৱর্ণ gDefaultConverterNameBuffer[UCNV_MAX_CONVERTER_NAME_LENGTH + 1]; /* +1 for nullptr */
স্থির ধ্রুৱ ৱর্ণ *gDefaultConverterName = nullptr;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
স্থির ধ্রুৱ UConverterSharedData *gDefaultAlgorithmicSharedData = nullptr;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
স্থির UBool gDefaultConverterContainsOption;

#endif  /* !U_CHARSET_IS_UTF8 */

স্থির ধ্রুৱ ৱর্ণ DATA_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
স্থির শূন্য
ucnv_flushAvailableConverterCache() {
    g_Available_Converter_গুনতি = 0;
    যদি (gAvailableConverters) {
        uprv_free(const_cast<ৱর্ণ**>(gAvailableConverters));
        gAvailableConverters = nullptr;
    }
    gAvailableConvertersInitOnce.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
স্থির UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    যদি (SHARED_DATA_HASHTABLE != nullptr && uhash_গুনতি(SHARED_DATA_HASHTABLE) == 0) {
        uhash_close(SHARED_DATA_HASHTABLE);
        SHARED_DATA_HASHTABLE = nullptr;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flushAvailableConverterCache();

#যদি !U_CHARSET_IS_UTF8
    gDefaultConverterName = nullptr;
    gDefaultConverterNameBuffer[0] = 0;
    gDefaultConverterContainsOption = মিছা;
    gDefaultAlgorithmicSharedData = nullptr;
#endif

    ঘূৰাই (SHARED_DATA_HASHTABLE == nullptr);
}

U_CAPI শূন্য U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_registerCleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

স্থির UBool U_CALLCONV
isCnvAcceptable(শূন্য * /*context*/,
                ধ্রুৱ ৱর্ণ * /*type*/, ধ্রুৱ ৱর্ণ * /*name*/,
                ধ্রুৱ UDataInfo *pInfo) {
    ঘূৰাই
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
স্থির UConverterSharedData*
ucnv_data_unFlattenClone(UConverterLoadArgs *pArgs, UDataMemory *pData, UErrorCode *status)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ধ্রুৱ uint8_t* raw = static_cast<ধ্রুৱ uint8_t*>(udata_getMemory(pData));
    ধ্রুৱ UConverterStaticData* source = reinterpret_cast<ধ্রুৱ UConverterStaticData*>(raw);
    UConverterSharedData *data;
    UConverterType type = static_cast<UConverterType>(source->conversionType);

    যদি(U_FAILURE(*status))
        ঘূৰাই nullptr;

    যদি (static_cast<uint16_t>(type) >= UCNV_সংখ্যা_OF_SUPPORTED_CONVERTER_TYPES ||
        converterData[type] == nullptr ||
        !converterData[type]->isReferenceCounted ||
        converterData[type]->referenceCounter != 1 ||
        source->structSize != পরিমাণ(UConverterStaticData))
    {
        *status = U_INVALID_TABLE_FORMAT;
        ঘূৰাই nullptr;
    }

    data = static_cast<UConverterSharedData*>(uprv_malloc(পরিমাণ(UConverterSharedData)));
    যদি(data == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(data, converterData[type], পরিমাণ(UConverterSharedData));

    data->staticData = source;

    data->sharedDataCached = মিছা;

    /* fill in fields from the loaded data */
    data->dataMemory = (শূন্য*)pData; /* for future use */

    যদি(data->impl->load != nullptr) {
        data->impl->load(data, pArgs, raw + source->structSize, status);
        যদি(U_FAILURE(*status)) {
            uprv_free(data);
            ঘূৰাই nullptr;
        }
    }
    ঘূৰাই data;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
স্থির UConverterSharedData *createConverterFromFile(UConverterLoadArgs *pArgs, UErrorCode * err)
{
    UDataMemory *data;
    UConverterSharedData *sharedData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_LOAD);

    যদি (U_FAILURE (*err)) {
        UTRACE_EXIT_STATUS(*err);
        ঘূৰাই nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", pArgs->name, pArgs->pkg);

    data = udata_openChoice(pArgs->pkg, DATA_TYPE, pArgs->name, isCnvAcceptable, nullptr, err);
    যদি(U_FAILURE(*err))
    {
        UTRACE_EXIT_STATUS(*err);
        ঘূৰাই nullptr;
    }

    sharedData = ucnv_data_unFlattenClone(pArgs, data, err);
    যদি(U_FAILURE(*err))
    {
        udata_close(data);
        UTRACE_EXIT_STATUS(*err);
        ঘূৰাই nullptr;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_STATUS(sharedData, *err);
    ঘূৰাই sharedData;
}

/*returns a converter type from a string
 */
স্থির ধ্রুৱ UConverterSharedData *
getAlgorithmicTypeFromName(ধ্রুৱ ৱর্ণ *realName)
{
    uint32_t mid, start, limit;
    uint32_t lastMid;
    পূর্ণাঙ্ক result;
    ৱর্ণ strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    start = 0;
    limit = UPRV_LENGTHOF(cnvNameType);
    mid = limit;
    lastMid = UINT32_MAX;

    বাবে (;;) {
        mid = (start + limit) / 2;
        যদি (lastMid == mid) {   /* Have we moved? */
            ভঙ্গ;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        result = uprv_strcmp(strippedName, cnvNameType[mid].name);

        যদি (result < 0) {
            limit = mid;
        } অন্যথা যদি (result > 0) {
            start = mid;
        } অন্যথা {
            ঘূৰাই converterData[cnvNameType[mid].type];
        }
    }

    ঘূৰাই nullptr;
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
স্থির শূন্য
ucnv_shareConverterData(UConverterSharedData * data)
{
    UErrorCode err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    যদি (SHARED_DATA_HASHTABLE == nullptr)
    {
        SHARED_DATA_HASHTABLE = uhash_openSize(uhash_hashChars, uhash_compareChars, nullptr,
                            ucnv_io_গুনতি_Known_Converters(&err)*UCNV_CACHE_LOAD_FACTOR,
                            &err);
        ucnv_enableCleanup();

        যদি (U_FAILURE(err))
            ঘূৰাই;
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
    data->sharedDataCached = সঁচা;

    uhash_put(SHARED_DATA_HASHTABLE,
            (শূন্য*) data->staticData->name, /* Okay to cast away const as long as
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
স্থির UConverterSharedData *
ucnv_getSharedConverterData(ধ্রুৱ ৱর্ণ *name)
{
    /*special case when no Table has yet been created we return nullptr */
    যদি (SHARED_DATA_HASHTABLE == nullptr)
    {
        ঘূৰাই nullptr;
    }
    অন্যথা
    {
        UConverterSharedData *rc;

        rc = static_cast<UConverterSharedData*>(uhash_get(SHARED_DATA_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        ঘূৰাই rc;
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
স্থির UBool
ucnv_deleteSharedConverterData(UConverterSharedData * deadSharedData)
{
    UTRACE_ENTRY_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", deadSharedData->staticData->name, deadSharedData);

    যদি (deadSharedData->referenceCounter > 0) {
        UTRACE_EXIT_মান((int32_t)মিছা);
        ঘূৰাই মিছা;
    }

    যদি (deadSharedData->impl->unload != nullptr) {
        deadSharedData->impl->unload(deadSharedData);
    }

    যদি(deadSharedData->dataMemory != nullptr)
    {
        UDataMemory *data = (UDataMemory*)deadSharedData->dataMemory;
        udata_close(data);
    }

    uprv_free(deadSharedData);

    UTRACE_EXIT_মান((int32_t)সঁচা);
    ঘূৰাই সঁচা;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverterSharedData *
ucnv_load(UConverterLoadArgs *pArgs, UErrorCode *err) {
    UConverterSharedData *mySharedConverterData;

    যদি(err == nullptr || U_FAILURE(*err)) {
        ঘূৰাই nullptr;
    }

    যদি(pArgs->pkg != nullptr && *pArgs->pkg != 0) {
        /* application-provided converters are not currently cached */
        ঘূৰাই createConverterFromFile(pArgs, err);
    }

    mySharedConverterData = ucnv_getSharedConverterData(pArgs->name);
    যদি (mySharedConverterData == nullptr)
    {
        /*Not cached, we need to stream it in from file */
        mySharedConverterData = createConverterFromFile(pArgs, err);
        যদি (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            ঘূৰাই nullptr;
        }
        অন্যথা যদি (!pArgs->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_shareConverterData(mySharedConverterData);
        }
    }
    অন্যথা
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        mySharedConverterData->referenceCounter++;
    }

    ঘূৰাই mySharedConverterData;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI শূন্য
ucnv_unload(UConverterSharedData *sharedData) {
    যদি(sharedData != nullptr) {
        যদি (sharedData->referenceCounter > 0) {
            sharedData->referenceCounter--;
        }

        যদি((sharedData->referenceCounter <= 0)&&(sharedData->sharedDataCached == মিছা)) {
            ucnv_deleteSharedConverterData(sharedData);
        }
    }
}

U_CFUNC শূন্য
ucnv_unloadSharedDataIfReady(UConverterSharedData *sharedData)
{
    যদি(sharedData != nullptr && sharedData->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        ucnv_unload(sharedData);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC শূন্য
ucnv_increment_Ref_গুনতি(UConverterSharedData *sharedData)
{
    যদি(sharedData != nullptr && sharedData->isReferenceCounted) {
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
স্থির শূন্য
parseConverterOptions(ধ্রুৱ ৱর্ণ *inName,
                      UConverterNamePieces *pPieces,
                      UConverterLoadArgs *pArgs,
                      UErrorCode *err)
{
    ৱর্ণ *cnvName = pPieces->cnvName;
    ৱর্ণ c;
    int32_t len = 0;

    pArgs->name=inName;
    pArgs->locale=pPieces->locale;
    pArgs->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    যেতিয়ালৈ((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        যদি (++len>=UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *err = U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            ঘূৰাই;
        }
        *cnvName++=c;
        inName++;
    }
    *cnvName=0;
    pArgs->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    যেতিয়ালৈ((c=*inName)!=0) {
        যদি(c==UCNV_OPTION_SEP_CHAR) {
            ++inName;
        }

        /* inName is behind an option separator */
        যদি(uprv_strncmp(inName, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            ৱর্ণ *dest=pPieces->locale;

            /* copy the locale option value */
            inName+=7;
            len=0;
            যেতিয়ালৈ((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++inName;

                যদি(++len>=ULOC_FULLNAME_CAPACITY) {
                    *err=U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    ঘূৰাই;
                }

                *dest++=c;
            }
            *dest=0;
        } অন্যথা যদি(uprv_strncmp(inName, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            inName+=8;
            c=*inName;
            যদি(c==0) {
                pArgs->options=(pPieces->options&=~UCNV_OPTION_VERSION);
                ঘূৰাই;
            } অন্যথা যদি (static_cast<uint8_t>(c - '0') < 10) {
                pArgs->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_VERSION) | static_cast<uint32_t>(c - '0');
                ++inName;
            }
        } অন্যথা যদি(uprv_strncmp(inName, "swaplfnl", 8)==0) {
            inName+=8;
            pArgs->options=(pPieces->options|=UCNV_OPTION_SWAP_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } অন্যথা {
            /* ignore any other options until we define some */
            যেতিয়ালৈ(((c = *inName++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            যদি(c==0) {
                ঘূৰাই;
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
ucnv_loadSharedData(ধ্রুৱ ৱর্ণ *converterName,
                    UConverterNamePieces *pPieces,
                    UConverterLoadArgs *pArgs,
                    UErrorCode * err) {
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args;
    UConverterSharedData *mySharedConverterData = nullptr;
    UErrorCode internalErrorCode = U_ZERO_ERROR;
    UBool mayContainOption = সঁচা;
    UBool checkForAlgorithmic = সঁচা;

    যদি (U_FAILURE (*err)) {
        ঘূৰাই nullptr;
    }

    যদি(pPieces == nullptr) {
        যদি(pArgs != nullptr) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_INTERNAL_PROGRAM_ERROR;
            ঘূৰাই nullptr;
        }
        pPieces = &স্তূপ_Pieces;
    }
    যদি(pArgs == nullptr) {
        uprv_memset(&স্তূপ_Args, 0, পরিমাণ(স্তূপ_Args));
        স্তূপ_Args.size = (int32_t)পরিমাণ(স্তূপ_Args);
        pArgs = &স্তূপ_Args;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    pArgs->name = converterName;
    pArgs->locale = pPieces->locale;
    pArgs->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    যদি (converterName == nullptr) {
#যদি U_CHARSET_IS_UTF8
        pArgs->name = "UTF-8";
        ঘূৰাই (UConverterSharedData *)converterData[UCNV_UTF8];
#অন্যথা
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        pArgs->name = ucnv_getDefaultName();
        যদি (pArgs->name == nullptr) {
            *err = U_MISSING_RESOURCE_ERROR;
            ঘূৰাই nullptr;
        }
        mySharedConverterData = (UConverterSharedData *)gDefaultAlgorithmicSharedData;
        checkForAlgorithmic = মিছা;
        mayContainOption = gDefaultConverterContainsOption;
        /* the default converter name is already canonical */
#endif
    }
    অন্যথা যদি(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        pArgs->name = "UTF-8";
        ঘূৰাই (UConverterSharedData *)converterData[UCNV_UTF8];
    }
    অন্যথা {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, pArgs, err);
        যদি (U_FAILURE(*err)) {
            /* Very bad name used. */
            ঘূৰাই nullptr;
        }

        /* get the canonical converter name */
        pArgs->name = ucnv_io_getConverterName(pArgs->name, &mayContainOption, &internalErrorCode);
        যদি (U_FAILURE(internalErrorCode) || pArgs->name == nullptr) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            pArgs->name = pPieces->cnvName;
        } অন্যথা যদি (internalErrorCode == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    যদি(mayContainOption && pArgs->name != pPieces->cnvName) {
        parseConverterOptions(pArgs->name, pPieces, pArgs, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    যদি (checkForAlgorithmic) {
        mySharedConverterData = (UConverterSharedData *)getAlgorithmicTypeFromName(pArgs->name);
    }
    যদি (mySharedConverterData == nullptr)
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
        যদি (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            ঘূৰাই nullptr;
        }
    }

    ঘূৰাই mySharedConverterData;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ধ্রুৱ ৱর্ণ* converterName,
                     UErrorCode* err)
{
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    যদি(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        mySharedConverterData = ucnv_loadSharedData(converterName, &স্তূপ_Pieces, &স্তূপ_Args, err);

        myUConverter = ucnv_createConverterFromSharedData(
            myUConverter, mySharedConverterData,
            &স্তূপ_Args,
            err);

        যদি(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
            ঘূৰাই myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_STATUS(*err);
    ঘূৰাই nullptr;
}

U_CFUNC UBool
ucnv_canCreateConverter(ধ্রুৱ ৱর্ণ *converterName, UErrorCode *err) {
    UConverter myUConverter;
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    যদি(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        স্তূপ_Args.onlyTestIsLoadable=সঁচা;
        mySharedConverterData = ucnv_loadSharedData(converterName, &স্তূপ_Pieces, &স্তূপ_Args, err);
        ucnv_createConverterFromSharedData(
            &myUConverter, mySharedConverterData,
            &স্তূপ_Args,
            err);
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
    }

    UTRACE_EXIT_STATUS(*err);
    ঘূৰাই U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ধ্রুৱ ৱর্ণ* locale,
                                uint32_t options,
                                UErrorCode* err) {
    UConverter *cnv;
    ধ্রুৱ UConverterSharedData *sharedData;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    যদি(type<0 || UCNV_সংখ্যা_OF_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        ঘূৰাই nullptr;
    }

    sharedData = converterData[type];
    যদি(sharedData == nullptr || sharedData->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        ঘূৰাই nullptr;
    }

    স্তূপ_Args.name = "";
    স্তূপ_Args.options = options;
    স্তূপ_Args.locale=locale;
    cnv = ucnv_createConverterFromSharedData(
            myUConverter, const_cast<UConverterSharedData*>(sharedData),
            &স্তূপ_Args, err);

    UTRACE_EXIT_PTR_STATUS(cnv, *err);
    ঘূৰাই cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ধ্রুৱ ৱর্ণ *packageName, ধ্রুৱ ৱর্ণ *converterName, UErrorCode * err)
{
    UConverter *myUConverter;
    UConverterSharedData *mySharedConverterData;
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_PACKAGE);

    যদি(U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        ঘূৰাই nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    স্তূপ_Pieces.cnvName[0] = 0;
    স্তূপ_Pieces.locale[0] = 0;
    স্তূপ_Pieces.options = 0;
    parseConverterOptions(converterName, &স্তূপ_Pieces, &স্তূপ_Args, err);
    যদি (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_STATUS(*err);
        ঘূৰাই nullptr;
    }
    স্তূপ_Args.nestedLoads=1;
    স্তূপ_Args.pkg=packageName;

    /* open the data, unflatten the shared structure */
    mySharedConverterData = createConverterFromFile(&স্তূপ_Args, err);

    যদি (U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        ঘূৰাই nullptr;
    }

    /* create the actual converter */
    myUConverter = ucnv_createConverterFromSharedData(nullptr, mySharedConverterData, &স্তূপ_Args, err);

    যদি (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_STATUS(*err);
        ঘূৰাই nullptr;
    }

    UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
    ঘূৰাই myUConverter;
}


U_CFUNC UConverter*
ucnv_createConverterFromSharedData(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverterSharedData* mySharedConverterData,
                                   UConverterLoadArgs* pArgs,
                                   UErrorCode* err)
{
    UBool isCopyLocal;

    যদি(U_FAILURE(*err)) {
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
        ঘূৰাই myUConverter;
    }
    যদি(myUConverter == nullptr)
    {
        myUConverter = (UConverter *) uprv_malloc (পরিমাণ (UConverter));
        যদি(myUConverter == nullptr)
        {
            *err = U_MEMORY_ALLOCATION_ERROR;
            ucnv_unloadSharedDataIfReady(mySharedConverterData);
            ঘূৰাই nullptr;
        }
        isCopyLocal = মিছা;
    } অন্যথা {
        isCopyLocal = সঁচা;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, পরিমাণ(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->sharedData = mySharedConverterData;
    myUConverter->options = pArgs->options;
    যদি(!pArgs->onlyTestIsLoadable) {
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

    যদি(mySharedConverterData->impl->open != nullptr) {
        mySharedConverterData->impl->open(myUConverter, pArgs, err);
        যদি(U_FAILURE(*err) && !pArgs->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            ঘূৰাই nullptr;
        }
    }

    ঘূৰাই myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverterSharedData *mySharedData = nullptr;
    int32_t pos;
    int32_t table_Deleted_অঙ্ক = 0;
    ধ্রুৱ UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, remaining;

    UTRACE_ENTRY_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    যদি (SHARED_DATA_HASHTABLE == nullptr) {
        UTRACE_EXIT_মান((int32_t)0);
        ঘূৰাই 0;
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
    কৰা {
        remaining = 0;
        pos = UHASH_FIRST;
        যেতিয়ালৈ ((e = uhash_nextElement (SHARED_DATA_HASHTABLE, &pos)) != nullptr)
        {
            mySharedData = (UConverterSharedData *) e->value.pointer;
            /*deletes only if reference counter == 0 */
            যদি (mySharedData->referenceCounter == 0)
            {
                table_Deleted_অঙ্ক++;

                UCNV_DEBUG_LOG("del",mySharedData->staticData->name,mySharedData);

                uhash_removeElement(SHARED_DATA_HASHTABLE, e);
                mySharedData->sharedDataCached = মিছা;
                ucnv_deleteSharedConverterData (mySharedData);
            } অন্যথা {
                ++remaining;
            }
        }
    } যেতিয়ালৈ(++i == 1 && remaining > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", remaining);

    UTRACE_EXIT_মান(table_Deleted_অঙ্ক);
    ঘূৰাই table_Deleted_অঙ্ক;
}

/* available converters list --------------------------------------------------- */

স্থির শূন্য U_CALLCONV initAvailableConvertersList(UErrorCode &errCode) {
    U_ASSERT(g_Available_Converter_গুনতি == 0);
    U_ASSERT(gAvailableConverters == nullptr);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_openAllNames(&errCode);
    int32_t all_Converter_গুনতি = uenum_গুনতি(allConvEnum, &errCode);
    যদি (U_FAILURE(errCode)) {
        ঘূৰাই;
    }

    /* We can't have more than "*converterTable" converters to open */
    gAvailableConverters = static_cast<ধ্রুৱ ৱর্ণ**>(uprv_malloc(all_Converter_গুনতি * পরিমাণ(ৱর্ণ*)));
    যদি (!gAvailableConverters) {
        errCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UErrorCode localStatus = U_ZERO_ERROR;
    UConverter tempConverter;
    ucnv_close(ucnv_createConverter(&tempConverter, nullptr, &localStatus));

    g_Available_Converter_গুনতি = 0;

    বাবে (int32_t idx = 0; idx < all_Converter_গুনতি; idx++) {
        localStatus = U_ZERO_ERROR;
        ধ্রুৱ ৱর্ণ *converterName = uenum_next(allConvEnum, nullptr, &localStatus);
        যদি (ucnv_canCreateConverter(converterName, &localStatus)) {
            gAvailableConverters[g_Available_Converter_গুনতি++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


স্থির UBool haveAvailableConverterList(UErrorCode *pErrorCode) {
    umtx_initOnce(gAvailableConvertersInitOnce, &initAvailableConvertersList, *pErrorCode);
    ঘূৰাই U_SUCCESS(*pErrorCode);
}

U_CFUNC uint16_t
ucnv_bld_গুনতি_Available_Converters(UErrorCode *pErrorCode) {
    যদি (haveAvailableConverterList(pErrorCode)) {
        ঘূৰাই g_Available_Converter_গুনতি;
    }
    ঘূৰাই 0;
}

U_CFUNC ধ্রুৱ ৱর্ণ *
ucnv_bld_getAvailableConverter(uint16_t n, UErrorCode *pErrorCode) {
    যদি (haveAvailableConverterList(pErrorCode)) {
        যদি (n < g_Available_Converter_গুনতি) {
            ঘূৰাই gAvailableConverters[n];
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }
    ঘূৰাই nullptr;
}

/* default converter name --------------------------------------------------- */

#যদি !U_CHARSET_IS_UTF8
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
স্থির অন্তঃস্থ শূন্য
internalSetName(ধ্রুৱ ৱর্ণ *name, UErrorCode *status) {
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool containsOption = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != nullptr);
    ধ্রুৱ UConverterSharedData *algorithmicSharedData;

    স্তূপ_Args.name = name;
    যদি(containsOption) {
        স্তূপ_Pieces.cnvName[0] = 0;
        স্তূপ_Pieces.locale[0] = 0;
        স্তূপ_Pieces.options = 0;
        parseConverterOptions(name, &স্তূপ_Pieces, &স্তূপ_Args, status);
        যদি(U_FAILURE(*status)) {
            ঘূৰাই;
        }
    }
    algorithmicSharedData = getAlgorithmicTypeFromName(স্তূপ_Args.name);

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

U_CAPI ধ্রুৱ ৱর্ণ*  U_EXPORT2
ucnv_getDefaultName() {
#যদি U_CHARSET_IS_UTF8
    ঘূৰাই "UTF-8";
#অন্যথা
    /* local variable to be thread-safe */
    ধ্রুৱ ৱর্ণ *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex lock(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    যদি(name==nullptr) {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        যদি(name != nullptr) {
            cnv = ucnv_open(name, &errorCode);
            যদি(U_SUCCESS(errorCode) && cnv != nullptr) {
                name = ucnv_getName(cnv, &errorCode);
            }
        }

        যদি(name == nullptr || name[0] == 0
            || U_FAILURE(errorCode) || cnv == nullptr
            || uprv_strlen(name)>=পরিমাণ(gDefaultConverterNameBuffer))
        {
            /* Panic time, let's use a fallback. */
#যদি (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#নোচেদ্যদি U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_SWAP_LFNL_OPTION_STRING;
#অন্যথা
            name = "ibm-37_P100-1995";
#endif
        }

        internalSetName(name, &errorCode);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    ঘূৰাই name;
#endif
}

#যদি U_CHARSET_IS_UTF8
U_CAPI শূন্য U_EXPORT2 ucnv_setDefaultName(ধ্রুৱ ৱর্ণ *) {}
#অন্যথা
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI শূন্য U_EXPORT2
ucnv_setDefaultName(ধ্রুৱ ৱর্ণ *converterName) {
    যদি(converterName==nullptr) {
        /* reset to the default codepage */
        gDefaultConverterName=nullptr;
    } অন্যথা {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;
        ধ্রুৱ ৱর্ণ *name = nullptr;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &errorCode);
        যদি(U_SUCCESS(errorCode) && cnv != nullptr) {
            name = ucnv_getName(cnv, &errorCode);
        }

        যদি(U_SUCCESS(errorCode) && name!=nullptr) {
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

#যদি !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_swap(ধ্রুৱ UDataSwapper *ds,
          ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
          UErrorCode *pErrorCode) {
    ধ্রুৱ UDataInfo *pInfo;
    int32_t headerSize;

    ধ্রুৱ uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t offset, count, staticDataSize;
    int32_t size;

    ধ্রুৱ UConverterStaticData *inStaticData;
    UConverterStaticData *outStaticData;

    ধ্রুৱ _MBCSHeader *inMBCSHeader;
    _MBCSHeader *outMBCSHeader;
    _MBCSHeader mbcsHeader;
    uint32_t mbcsHeaderLength;
    UBool noFromU=মিছা;

    uint8_t outputType;

    int32_t maxFastUChar, mbcsIndexLength;

    ধ্রুৱ int32_t *inExtIndexes;
    int32_t extOffset;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুৱ UDataInfo *)((ধ্রুৱ ৱর্ণ *)inData+4);
    যদি(!(
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
        ঘূৰাই 0;
    }

    inBytes=(ধ্রুৱ uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    inStaticData=(ধ্রুৱ UConverterStaticData *)inBytes;
    outStaticData=(UConverterStaticData *)outBytes;

    যদি(length<0) {
        staticDataSize=ds->readUInt32(inStaticData->structSize);
    } অন্যথা {
        length-=headerSize;
        যদি( length<(int32_t)পরিমাণ(UConverterStaticData) ||
            (uint32_t)length<(staticDataSize=ds->readUInt32(inStaticData->structSize))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ঘূৰাই 0;
        }
    }

    যদি(length>=0) {
        /* swap the static data */
        যদি(inStaticData!=outStaticData) {
            uprv_memcpy(outStaticData, inStaticData, staticDataSize);
        }

        ds->swapArray32(ds, &inStaticData->structSize, 4,
                           &outStaticData->structSize, pErrorCode);
        ds->swapArray32(ds, &inStaticData->codepage, 4,
                           &outStaticData->codepage, pErrorCode);

        ds->swapInvChars(ds, inStaticData->name, (int32_t)uprv_strlen(inStaticData->name),
                            outStaticData->name, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            ঘূৰাই 0;
        }
    }

    inBytes+=staticDataSize;
    যদি (outBytes != nullptr) outBytes+=staticDataSize;
    যদি(length>=0) {
        length-=(int32_t)staticDataSize;
    }

    /* check for supported conversionType values */
    যদি(inStaticData->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        inMBCSHeader=(ধ্রুৱ _MBCSHeader *)inBytes;
        outMBCSHeader=(_MBCSHeader *)outBytes;

        যদি(0<=length && length<(int32_t)পরিমাণ(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ঘূৰাই 0;
        }
        যদি(inMBCSHeader->version[0]==4 && inMBCSHeader->version[1]>=1) {
            mbcsHeaderLength=MBCS_HEADER_V4_LENGTH;
        } অন্যথা যদি(inMBCSHeader->version[0]==5 && inMBCSHeader->version[1]>=3 &&
                  ((mbcsHeader.options=ds->readUInt32(inMBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_MASK)==0
        ) {
            mbcsHeaderLength=mbcsHeader.options&MBCS_OPT_LENGTH_MASK;
            noFromU = (mbcsHeader.options & MBCS_OPT_NO_FROM_U) != 0;
        } অন্যথা {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             inMBCSHeader->version[0], inMBCSHeader->version[1]);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ঘূৰাই 0;
        }

        uprv_memcpy(mbcsHeader.version, inMBCSHeader->version, 4);
        mbcsHeader.গুনতি_States=         ds->readUInt32(inMBCSHeader->গুনতি_States);
        mbcsHeader.গুনতি_To_UFallbacks=   ds->readUInt32(inMBCSHeader->গুনতি_To_UFallbacks);
        mbcsHeader.offsetToUCodeUnits=  ds->readUInt32(inMBCSHeader->offsetToUCodeUnits);
        mbcsHeader.offsetFromUTable=    ds->readUInt32(inMBCSHeader->offsetFromUTable);
        mbcsHeader.offsetFromUBytes=    ds->readUInt32(inMBCSHeader->offsetFromUBytes);
        mbcsHeader.flags=               ds->readUInt32(inMBCSHeader->flags);
        mbcsHeader.fromUBytesLength=    ds->readUInt32(inMBCSHeader->fromUBytesLength);
        /* mbcsHeader.options have been read above */

        extOffset=(int32_t)(mbcsHeader.flags>>8);
        outputType=(uint8_t)mbcsHeader.flags;
        যদি(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ঘূৰাই 0;
        }

        /* make sure that the output type is known */
        ৱিকল্প(outputType) {
        পক্ষ MBCS_OUTPUT_1:
        পক্ষ MBCS_OUTPUT_2:
        পক্ষ MBCS_OUTPUT_3:
        পক্ষ MBCS_OUTPUT_4:
        পক্ষ MBCS_OUTPUT_3_EUC:
        পক্ষ MBCS_OUTPUT_4_EUC:
        পক্ষ MBCS_OUTPUT_2_SISO:
        পক্ষ MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            ভঙ্গ;
        স্ৱভাৱ:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ঘূৰাই 0;
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
        যদি( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcsHeader.version[1]>=3 && (maxFastUChar=mbcsHeader.version[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcsIndexLength=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        যদি(extOffset==0) {
            size=(int32_t)(mbcsHeader.offsetFromUBytes+mbcsIndexLength);
            যদি(!noFromU) {
                size+=(int32_t)mbcsHeader.fromUBytesLength;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            inExtIndexes=nullptr;
        } অন্যথা {
            /* there is extension data after the base data, see ucnv_ext.h */
            যদি(length>=0 && length<(extOffset+UCNV_EXT_INDEXES_MIN_LENGTH*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ঘূৰাই 0;
            }

            inExtIndexes=(ধ্রুৱ int32_t *)(inBytes+extOffset);
            size=extOffset+udata_readInt32(ds, inExtIndexes[UCNV_EXT_SIZE]);
        }

        যদি(length>=0) {
            যদি(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ঘূৰাই 0;
            }

            /* copy the data for inaccessible bytes */
            যদি(inBytes!=outBytes) {
                uprv_memcpy(outBytes, inBytes, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcsHeaderLength*4;
            ds->swapArray32(ds, &inMBCSHeader->গুনতি_States, count-4,
                               &outMBCSHeader->গুনতি_States, pErrorCode);

            যদি(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ধ্রুৱ ৱর্ণ *inBaseName=(ধ্রুৱ ৱর্ণ *)inBytes+count;
                ৱর্ণ *outBaseName=(ৱর্ণ *)outBytes+count;
                ds->swapInvChars(ds, inBaseName, (int32_t)uprv_strlen(inBaseName),
                                    outBaseName, pErrorCode);
            } অন্যথা {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                offset=count;
                count=mbcsHeader.গুনতি_States*1024;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the toUFallbacks[] */
                offset+=count;
                count=mbcsHeader.গুনতি_To_UFallbacks*8;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the unicodeCodeUnits[] */
                offset=mbcsHeader.offsetToUCodeUnits;
                count=mbcsHeader.offsetFromUTable-offset;
                ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* offset to the stage 1 table, independent of the outputType */
                offset=mbcsHeader.offsetFromUTable;

                যদি(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcsHeader.offsetFromUBytes-offset)+mbcsHeader.fromUBytesLength;
                    ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                       outBytes+offset, pErrorCode);
                } অন্যথা {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    যদি(inStaticData->unicodeMask&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } অন্যথা {
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
                    ৱিকল্প(outputType) {
                    পক্ষ MBCS_OUTPUT_2:
                    পক্ষ MBCS_OUTPUT_3_EUC:
                    পক্ষ MBCS_OUTPUT_2_SISO:
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        ভঙ্গ;
                    পক্ষ MBCS_OUTPUT_4:
                        ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        ভঙ্গ;
                    স্ৱভাৱ:
                        /* just uint8_t[], nothing to swap */
                        ভঙ্গ;
                    }

                    যদি(mbcsIndexLength!=0) {
                        offset+=count;
                        count=mbcsIndexLength;
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                    }
                }
            }

            যদি(extOffset!=0) {
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
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_মানসমূহ_INDEX]);
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
    } অন্যথা {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         inStaticData->conversionType);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ঘূৰাই 0;
    }

    ঘূৰাই headerSize+(int32_t)staticDataSize+size;
}

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#endif
