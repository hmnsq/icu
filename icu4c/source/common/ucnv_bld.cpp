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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_CONVERSION

#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "putilimp.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "utracimp.h"
#અન્તર્ભાવ "ucnv_io.h"
#અન્તર્ભાવ "ucnv_bld.h"
#અન્તર્ભાવ "ucnvmbcs.h"
#અન્તર્ભાવ "ucnv_ext.h"
#અન્તર્ભાવ "ucnv_cnv.h"
#અન્તર્ભાવ "ucnv_imp.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "ustr_cnv.h"


#જો 0
#અન્તર્ભાવ <stdio.h>
બાહ્ય શૂન્ય UCNV_DEBUG_LOG(અક્ષર *what, અક્ષર *who, શૂન્ય *p, પૂર્ણાંક l);
#define UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#નહીંતર
# define UCNV_DEBUG_LOG(x,y,z)
#endif

સ્થિર અચલ UConverterSharedData * અચલ
converterData[UCNV_સંખ્યા_OF_SUPPORTED_CONVERTER_TYPES]={
    nullptr, nullptr,

#જો UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#નહીંતર
    &_MBCSData,
#endif

    &_Latin1Data,
    &_UTF8Data, &_UTF16BEData, &_UTF16LEData,
#જો UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr,
#નહીંતર
    &_UTF32BEData, &_UTF32LEData,
#endif
    nullptr,

#જો UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#નહીંતર
    &_ISO2022Data,
#endif

#જો UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr,
#નહીંતર
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#endif

#જો UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#નહીંતર
    &_SCSUData,
#endif


#જો UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#નહીંતર
    &_ISCIIData,
#endif

    &_ASCIIData,
#જો UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, &_UTF16Data, nullptr, nullptr, nullptr,
#નહીંતર
    &_UTF7Data, &_Bocu1Data, &_UTF16Data, &_UTF32Data, &_CESU8Data, &_IMAPData,
#endif

#જો UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#નહીંતર
    &_CompoundTextData
#endif
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
સ્થિર સંરચના {
  અચલ અક્ષર *name;
  અચલ UConverterType type;
} અચલ cnvNameType[] = {
#જો !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#endif
#જો !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#endif
#જો !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#endif
#જો !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#endif
#જો !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#endif
  { "iso88591", UCNV_LATIN_1 },
#જો !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#જો !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#endif
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#જો U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#નહીંતર
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#endif
#જો !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#જો U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#નહીંતર
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#endif
#endif
#જો !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#endif
  { "utf8", UCNV_UTF8 },
#જો !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_TEXT}
#endif
};


/*initializes some global variables */
સ્થિર UHashtable *SHARED_DATA_HASHTABLE = nullptr;
સ્થિર icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

સ્થિર અચલ અક્ષર **gAvailableConverters = nullptr;
સ્થિર uint16_t g_Available_Converter_ગણતરી = 0;
સ્થિર icu::UInitOnce gAvailableConvertersInitOnce {};

#જો !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
સ્થિર અક્ષર gDefaultConverterNameBuffer[UCNV_MAX_CONVERTER_NAME_LENGTH + 1]; /* +1 for nullptr */
સ્થિર અચલ અક્ષર *gDefaultConverterName = nullptr;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
સ્થિર અચલ UConverterSharedData *gDefaultAlgorithmicSharedData = nullptr;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
સ્થિર UBool gDefaultConverterContainsOption;

#endif  /* !U_CHARSET_IS_UTF8 */

સ્થિર અચલ અક્ષર DATA_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
સ્થિર શૂન્ય
ucnv_flushAvailableConverterCache() {
    g_Available_Converter_ગણતરી = 0;
    જો (gAvailableConverters) {
        uprv_free(const_cast<અક્ષર**>(gAvailableConverters));
        gAvailableConverters = nullptr;
    }
    gAvailableConvertersInitOnce.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
સ્થિર UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    જો (SHARED_DATA_HASHTABLE != nullptr && uhash_ગણતરી(SHARED_DATA_HASHTABLE) == 0) {
        uhash_close(SHARED_DATA_HASHTABLE);
        SHARED_DATA_HASHTABLE = nullptr;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flushAvailableConverterCache();

#જો !U_CHARSET_IS_UTF8
    gDefaultConverterName = nullptr;
    gDefaultConverterNameBuffer[0] = 0;
    gDefaultConverterContainsOption = ખોટું;
    gDefaultAlgorithmicSharedData = nullptr;
#endif

    પાછા (SHARED_DATA_HASHTABLE == nullptr);
}

U_CAPI શૂન્ય U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_registerCleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

સ્થિર UBool U_CALLCONV
isCnvAcceptable(શૂન્ય * /*context*/,
                અચલ અક્ષર * /*type*/, અચલ અક્ષર * /*name*/,
                અચલ UDataInfo *pInfo) {
    પાછા
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
સ્થિર UConverterSharedData*
ucnv_data_unFlattenClone(UConverterLoadArgs *pArgs, UDataMemory *pData, UErrorCode *status)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    અચલ uint8_t* raw = static_cast<અચલ uint8_t*>(udata_getMemory(pData));
    અચલ UConverterStaticData* source = reinterpret_cast<અચલ UConverterStaticData*>(raw);
    UConverterSharedData *data;
    UConverterType type = static_cast<UConverterType>(source->conversionType);

    જો(U_FAILURE(*status))
        પાછા nullptr;

    જો (static_cast<uint16_t>(type) >= UCNV_સંખ્યા_OF_SUPPORTED_CONVERTER_TYPES ||
        converterData[type] == nullptr ||
        !converterData[type]->isReferenceCounted ||
        converterData[type]->referenceCounter != 1 ||
        source->structSize != કદ(UConverterStaticData))
    {
        *status = U_INVALID_TABLE_FORMAT;
        પાછા nullptr;
    }

    data = static_cast<UConverterSharedData*>(uprv_malloc(કદ(UConverterSharedData)));
    જો(data == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(data, converterData[type], કદ(UConverterSharedData));

    data->staticData = source;

    data->sharedDataCached = ખોટું;

    /* fill in fields from the loaded data */
    data->dataMemory = (શૂન્ય*)pData; /* for future use */

    જો(data->impl->load != nullptr) {
        data->impl->load(data, pArgs, raw + source->structSize, status);
        જો(U_FAILURE(*status)) {
            uprv_free(data);
            પાછા nullptr;
        }
    }
    પાછા data;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
સ્થિર UConverterSharedData *createConverterFromFile(UConverterLoadArgs *pArgs, UErrorCode * err)
{
    UDataMemory *data;
    UConverterSharedData *sharedData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_LOAD);

    જો (U_FAILURE (*err)) {
        UTRACE_EXIT_STATUS(*err);
        પાછા nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", pArgs->name, pArgs->pkg);

    data = udata_openChoice(pArgs->pkg, DATA_TYPE, pArgs->name, isCnvAcceptable, nullptr, err);
    જો(U_FAILURE(*err))
    {
        UTRACE_EXIT_STATUS(*err);
        પાછા nullptr;
    }

    sharedData = ucnv_data_unFlattenClone(pArgs, data, err);
    જો(U_FAILURE(*err))
    {
        udata_close(data);
        UTRACE_EXIT_STATUS(*err);
        પાછા nullptr;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_STATUS(sharedData, *err);
    પાછા sharedData;
}

/*returns a converter type from a string
 */
સ્થિર અચલ UConverterSharedData *
getAlgorithmicTypeFromName(અચલ અક્ષર *realName)
{
    uint32_t mid, start, limit;
    uint32_t lastMid;
    પૂર્ણાંક result;
    અક્ષર strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    start = 0;
    limit = UPRV_LENGTHOF(cnvNameType);
    mid = limit;
    lastMid = UINT32_MAX;

    માટે (;;) {
        mid = (start + limit) / 2;
        જો (lastMid == mid) {   /* Have we moved? */
            તોડો;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        result = uprv_strcmp(strippedName, cnvNameType[mid].name);

        જો (result < 0) {
            limit = mid;
        } નહીંતર જો (result > 0) {
            start = mid;
        } નહીંતર {
            પાછા converterData[cnvNameType[mid].type];
        }
    }

    પાછા nullptr;
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
સ્થિર શૂન્ય
ucnv_shareConverterData(UConverterSharedData * data)
{
    UErrorCode err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    જો (SHARED_DATA_HASHTABLE == nullptr)
    {
        SHARED_DATA_HASHTABLE = uhash_openSize(uhash_hashChars, uhash_compareChars, nullptr,
                            ucnv_io_ગણતરી_Known_Converters(&err)*UCNV_CACHE_LOAD_FACTOR,
                            &err);
        ucnv_enableCleanup();

        જો (U_FAILURE(err))
            પાછા;
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
    data->sharedDataCached = સાચું;

    uhash_put(SHARED_DATA_HASHTABLE,
            (શૂન્ય*) data->staticData->name, /* Okay to cast away const as long as
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
સ્થિર UConverterSharedData *
ucnv_getSharedConverterData(અચલ અક્ષર *name)
{
    /*special case when no Table has yet been created we return nullptr */
    જો (SHARED_DATA_HASHTABLE == nullptr)
    {
        પાછા nullptr;
    }
    નહીંતર
    {
        UConverterSharedData *rc;

        rc = static_cast<UConverterSharedData*>(uhash_get(SHARED_DATA_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        પાછા rc;
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
સ્થિર UBool
ucnv_deleteSharedConverterData(UConverterSharedData * deadSharedData)
{
    UTRACE_ENTRY_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", deadSharedData->staticData->name, deadSharedData);

    જો (deadSharedData->referenceCounter > 0) {
        UTRACE_EXIT_કિંમત((int32_t)ખોટું);
        પાછા ખોટું;
    }

    જો (deadSharedData->impl->unload != nullptr) {
        deadSharedData->impl->unload(deadSharedData);
    }

    જો(deadSharedData->dataMemory != nullptr)
    {
        UDataMemory *data = (UDataMemory*)deadSharedData->dataMemory;
        udata_close(data);
    }

    uprv_free(deadSharedData);

    UTRACE_EXIT_કિંમત((int32_t)સાચું);
    પાછા સાચું;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverterSharedData *
ucnv_load(UConverterLoadArgs *pArgs, UErrorCode *err) {
    UConverterSharedData *mySharedConverterData;

    જો(err == nullptr || U_FAILURE(*err)) {
        પાછા nullptr;
    }

    જો(pArgs->pkg != nullptr && *pArgs->pkg != 0) {
        /* application-provided converters are not currently cached */
        પાછા createConverterFromFile(pArgs, err);
    }

    mySharedConverterData = ucnv_getSharedConverterData(pArgs->name);
    જો (mySharedConverterData == nullptr)
    {
        /*Not cached, we need to stream it in from file */
        mySharedConverterData = createConverterFromFile(pArgs, err);
        જો (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            પાછા nullptr;
        }
        નહીંતર જો (!pArgs->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_shareConverterData(mySharedConverterData);
        }
    }
    નહીંતર
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        mySharedConverterData->referenceCounter++;
    }

    પાછા mySharedConverterData;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI શૂન્ય
ucnv_unload(UConverterSharedData *sharedData) {
    જો(sharedData != nullptr) {
        જો (sharedData->referenceCounter > 0) {
            sharedData->referenceCounter--;
        }

        જો((sharedData->referenceCounter <= 0)&&(sharedData->sharedDataCached == ખોટું)) {
            ucnv_deleteSharedConverterData(sharedData);
        }
    }
}

U_CFUNC શૂન્ય
ucnv_unloadSharedDataIfReady(UConverterSharedData *sharedData)
{
    જો(sharedData != nullptr && sharedData->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        ucnv_unload(sharedData);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC શૂન્ય
ucnv_increment_Ref_ગણતરી(UConverterSharedData *sharedData)
{
    જો(sharedData != nullptr && sharedData->isReferenceCounted) {
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
સ્થિર શૂન્ય
parseConverterOptions(અચલ અક્ષર *inName,
                      UConverterNamePieces *pPieces,
                      UConverterLoadArgs *pArgs,
                      UErrorCode *err)
{
    અક્ષર *cnvName = pPieces->cnvName;
    અક્ષર c;
    int32_t len = 0;

    pArgs->name=inName;
    pArgs->locale=pPieces->locale;
    pArgs->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    જ્યારે((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        જો (++len>=UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *err = U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            પાછા;
        }
        *cnvName++=c;
        inName++;
    }
    *cnvName=0;
    pArgs->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    જ્યારે((c=*inName)!=0) {
        જો(c==UCNV_OPTION_SEP_CHAR) {
            ++inName;
        }

        /* inName is behind an option separator */
        જો(uprv_strncmp(inName, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            અક્ષર *dest=pPieces->locale;

            /* copy the locale option value */
            inName+=7;
            len=0;
            જ્યારે((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++inName;

                જો(++len>=ULOC_FULLNAME_CAPACITY) {
                    *err=U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    પાછા;
                }

                *dest++=c;
            }
            *dest=0;
        } નહીંતર જો(uprv_strncmp(inName, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            inName+=8;
            c=*inName;
            જો(c==0) {
                pArgs->options=(pPieces->options&=~UCNV_OPTION_VERSION);
                પાછા;
            } નહીંતર જો (static_cast<uint8_t>(c - '0') < 10) {
                pArgs->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_VERSION) | static_cast<uint32_t>(c - '0');
                ++inName;
            }
        } નહીંતર જો(uprv_strncmp(inName, "swaplfnl", 8)==0) {
            inName+=8;
            pArgs->options=(pPieces->options|=UCNV_OPTION_SWAP_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } નહીંતર {
            /* ignore any other options until we define some */
            જ્યારે(((c = *inName++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            જો(c==0) {
                પાછા;
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
ucnv_loadSharedData(અચલ અક્ષર *converterName,
                    UConverterNamePieces *pPieces,
                    UConverterLoadArgs *pArgs,
                    UErrorCode * err) {
    UConverterNamePieces ઢગલો_Pieces;
    UConverterLoadArgs ઢગલો_Args;
    UConverterSharedData *mySharedConverterData = nullptr;
    UErrorCode internalErrorCode = U_ZERO_ERROR;
    UBool mayContainOption = સાચું;
    UBool checkForAlgorithmic = સાચું;

    જો (U_FAILURE (*err)) {
        પાછા nullptr;
    }

    જો(pPieces == nullptr) {
        જો(pArgs != nullptr) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_INTERNAL_PROGRAM_ERROR;
            પાછા nullptr;
        }
        pPieces = &ઢગલો_Pieces;
    }
    જો(pArgs == nullptr) {
        uprv_memset(&ઢગલો_Args, 0, કદ(ઢગલો_Args));
        ઢગલો_Args.size = (int32_t)કદ(ઢગલો_Args);
        pArgs = &ઢગલો_Args;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    pArgs->name = converterName;
    pArgs->locale = pPieces->locale;
    pArgs->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    જો (converterName == nullptr) {
#જો U_CHARSET_IS_UTF8
        pArgs->name = "UTF-8";
        પાછા (UConverterSharedData *)converterData[UCNV_UTF8];
#નહીંતર
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        pArgs->name = ucnv_getDefaultName();
        જો (pArgs->name == nullptr) {
            *err = U_MISSING_RESOURCE_ERROR;
            પાછા nullptr;
        }
        mySharedConverterData = (UConverterSharedData *)gDefaultAlgorithmicSharedData;
        checkForAlgorithmic = ખોટું;
        mayContainOption = gDefaultConverterContainsOption;
        /* the default converter name is already canonical */
#endif
    }
    નહીંતર જો(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        pArgs->name = "UTF-8";
        પાછા (UConverterSharedData *)converterData[UCNV_UTF8];
    }
    નહીંતર {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, pArgs, err);
        જો (U_FAILURE(*err)) {
            /* Very bad name used. */
            પાછા nullptr;
        }

        /* get the canonical converter name */
        pArgs->name = ucnv_io_getConverterName(pArgs->name, &mayContainOption, &internalErrorCode);
        જો (U_FAILURE(internalErrorCode) || pArgs->name == nullptr) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            pArgs->name = pPieces->cnvName;
        } નહીંતર જો (internalErrorCode == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    જો(mayContainOption && pArgs->name != pPieces->cnvName) {
        parseConverterOptions(pArgs->name, pPieces, pArgs, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    જો (checkForAlgorithmic) {
        mySharedConverterData = (UConverterSharedData *)getAlgorithmicTypeFromName(pArgs->name);
    }
    જો (mySharedConverterData == nullptr)
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
        જો (U_FAILURE (*err) || (mySharedConverterData == nullptr))
        {
            પાછા nullptr;
        }
    }

    પાછા mySharedConverterData;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     અચલ અક્ષર* converterName,
                     UErrorCode* err)
{
    UConverterNamePieces ઢગલો_Pieces;
    UConverterLoadArgs ઢગલો_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    જો(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        mySharedConverterData = ucnv_loadSharedData(converterName, &ઢગલો_Pieces, &ઢગલો_Args, err);

        myUConverter = ucnv_createConverterFromSharedData(
            myUConverter, mySharedConverterData,
            &ઢગલો_Args,
            err);

        જો(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
            પાછા myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_STATUS(*err);
    પાછા nullptr;
}

U_CFUNC UBool
ucnv_canCreateConverter(અચલ અક્ષર *converterName, UErrorCode *err) {
    UConverter myUConverter;
    UConverterNamePieces ઢગલો_Pieces;
    UConverterLoadArgs ઢગલો_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverterSharedData *mySharedConverterData;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    જો(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ઢગલો_Args.onlyTestIsLoadable=સાચું;
        mySharedConverterData = ucnv_loadSharedData(converterName, &ઢગલો_Pieces, &ઢગલો_Args, err);
        ucnv_createConverterFromSharedData(
            &myUConverter, mySharedConverterData,
            &ઢગલો_Args,
            err);
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
    }

    UTRACE_EXIT_STATUS(*err);
    પાછા U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                અચલ અક્ષર* locale,
                                uint32_t options,
                                UErrorCode* err) {
    UConverter *cnv;
    અચલ UConverterSharedData *sharedData;
    UConverterLoadArgs ઢગલો_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    જો(type<0 || UCNV_સંખ્યા_OF_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        પાછા nullptr;
    }

    sharedData = converterData[type];
    જો(sharedData == nullptr || sharedData->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        પાછા nullptr;
    }

    ઢગલો_Args.name = "";
    ઢગલો_Args.options = options;
    ઢગલો_Args.locale=locale;
    cnv = ucnv_createConverterFromSharedData(
            myUConverter, const_cast<UConverterSharedData*>(sharedData),
            &ઢગલો_Args, err);

    UTRACE_EXIT_PTR_STATUS(cnv, *err);
    પાછા cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(અચલ અક્ષર *packageName, અચલ અક્ષર *converterName, UErrorCode * err)
{
    UConverter *myUConverter;
    UConverterSharedData *mySharedConverterData;
    UConverterNamePieces ઢગલો_Pieces;
    UConverterLoadArgs ઢગલો_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_PACKAGE);

    જો(U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        પાછા nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ઢગલો_Pieces.cnvName[0] = 0;
    ઢગલો_Pieces.locale[0] = 0;
    ઢગલો_Pieces.options = 0;
    parseConverterOptions(converterName, &ઢગલો_Pieces, &ઢગલો_Args, err);
    જો (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_STATUS(*err);
        પાછા nullptr;
    }
    ઢગલો_Args.nestedLoads=1;
    ઢગલો_Args.pkg=packageName;

    /* open the data, unflatten the shared structure */
    mySharedConverterData = createConverterFromFile(&ઢગલો_Args, err);

    જો (U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        પાછા nullptr;
    }

    /* create the actual converter */
    myUConverter = ucnv_createConverterFromSharedData(nullptr, mySharedConverterData, &ઢગલો_Args, err);

    જો (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_STATUS(*err);
        પાછા nullptr;
    }

    UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
    પાછા myUConverter;
}


U_CFUNC UConverter*
ucnv_createConverterFromSharedData(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverterSharedData* mySharedConverterData,
                                   UConverterLoadArgs* pArgs,
                                   UErrorCode* err)
{
    UBool isCopyLocal;

    જો(U_FAILURE(*err)) {
        ucnv_unloadSharedDataIfReady(mySharedConverterData);
        પાછા myUConverter;
    }
    જો(myUConverter == nullptr)
    {
        myUConverter = (UConverter *) uprv_malloc (કદ (UConverter));
        જો(myUConverter == nullptr)
        {
            *err = U_MEMORY_ALLOCATION_ERROR;
            ucnv_unloadSharedDataIfReady(mySharedConverterData);
            પાછા nullptr;
        }
        isCopyLocal = ખોટું;
    } નહીંતર {
        isCopyLocal = સાચું;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, કદ(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->sharedData = mySharedConverterData;
    myUConverter->options = pArgs->options;
    જો(!pArgs->onlyTestIsLoadable) {
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

    જો(mySharedConverterData->impl->open != nullptr) {
        mySharedConverterData->impl->open(myUConverter, pArgs, err);
        જો(U_FAILURE(*err) && !pArgs->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            પાછા nullptr;
        }
    }

    પાછા myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverterSharedData *mySharedData = nullptr;
    int32_t pos;
    int32_t table_Deleted_અંક = 0;
    અચલ UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, remaining;

    UTRACE_ENTRY_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    જો (SHARED_DATA_HASHTABLE == nullptr) {
        UTRACE_EXIT_કિંમત((int32_t)0);
        પાછા 0;
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
    કરો {
        remaining = 0;
        pos = UHASH_FIRST;
        જ્યારે ((e = uhash_nextElement (SHARED_DATA_HASHTABLE, &pos)) != nullptr)
        {
            mySharedData = (UConverterSharedData *) e->value.pointer;
            /*deletes only if reference counter == 0 */
            જો (mySharedData->referenceCounter == 0)
            {
                table_Deleted_અંક++;

                UCNV_DEBUG_LOG("del",mySharedData->staticData->name,mySharedData);

                uhash_removeElement(SHARED_DATA_HASHTABLE, e);
                mySharedData->sharedDataCached = ખોટું;
                ucnv_deleteSharedConverterData (mySharedData);
            } નહીંતર {
                ++remaining;
            }
        }
    } જ્યારે(++i == 1 && remaining > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", remaining);

    UTRACE_EXIT_કિંમત(table_Deleted_અંક);
    પાછા table_Deleted_અંક;
}

/* available converters list --------------------------------------------------- */

સ્થિર શૂન્ય U_CALLCONV initAvailableConvertersList(UErrorCode &errCode) {
    U_ASSERT(g_Available_Converter_ગણતરી == 0);
    U_ASSERT(gAvailableConverters == nullptr);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_openAllNames(&errCode);
    int32_t all_Converter_ગણતરી = uenum_ગણતરી(allConvEnum, &errCode);
    જો (U_FAILURE(errCode)) {
        પાછા;
    }

    /* We can't have more than "*converterTable" converters to open */
    gAvailableConverters = static_cast<અચલ અક્ષર**>(uprv_malloc(all_Converter_ગણતરી * કદ(અક્ષર*)));
    જો (!gAvailableConverters) {
        errCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UErrorCode localStatus = U_ZERO_ERROR;
    UConverter tempConverter;
    ucnv_close(ucnv_createConverter(&tempConverter, nullptr, &localStatus));

    g_Available_Converter_ગણતરી = 0;

    માટે (int32_t idx = 0; idx < all_Converter_ગણતરી; idx++) {
        localStatus = U_ZERO_ERROR;
        અચલ અક્ષર *converterName = uenum_next(allConvEnum, nullptr, &localStatus);
        જો (ucnv_canCreateConverter(converterName, &localStatus)) {
            gAvailableConverters[g_Available_Converter_ગણતરી++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


સ્થિર UBool haveAvailableConverterList(UErrorCode *pErrorCode) {
    umtx_initOnce(gAvailableConvertersInitOnce, &initAvailableConvertersList, *pErrorCode);
    પાછા U_SUCCESS(*pErrorCode);
}

U_CFUNC uint16_t
ucnv_bld_ગણતરી_Available_Converters(UErrorCode *pErrorCode) {
    જો (haveAvailableConverterList(pErrorCode)) {
        પાછા g_Available_Converter_ગણતરી;
    }
    પાછા 0;
}

U_CFUNC અચલ અક્ષર *
ucnv_bld_getAvailableConverter(uint16_t n, UErrorCode *pErrorCode) {
    જો (haveAvailableConverterList(pErrorCode)) {
        જો (n < g_Available_Converter_ગણતરી) {
            પાછા gAvailableConverters[n];
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }
    પાછા nullptr;
}

/* default converter name --------------------------------------------------- */

#જો !U_CHARSET_IS_UTF8
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
સ્થિર અંતઃરેખ શૂન્ય
internalSetName(અચલ અક્ષર *name, UErrorCode *status) {
    UConverterNamePieces ઢગલો_Pieces;
    UConverterLoadArgs ઢગલો_Args=UCNV_LOAD_ARGS_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool containsOption = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != nullptr);
    અચલ UConverterSharedData *algorithmicSharedData;

    ઢગલો_Args.name = name;
    જો(containsOption) {
        ઢગલો_Pieces.cnvName[0] = 0;
        ઢગલો_Pieces.locale[0] = 0;
        ઢગલો_Pieces.options = 0;
        parseConverterOptions(name, &ઢગલો_Pieces, &ઢગલો_Args, status);
        જો(U_FAILURE(*status)) {
            પાછા;
        }
    }
    algorithmicSharedData = getAlgorithmicTypeFromName(ઢગલો_Args.name);

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

U_CAPI અચલ અક્ષર*  U_EXPORT2
ucnv_getDefaultName() {
#જો U_CHARSET_IS_UTF8
    પાછા "UTF-8";
#નહીંતર
    /* local variable to be thread-safe */
    અચલ અક્ષર *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex lock(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    જો(name==nullptr) {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        જો(name != nullptr) {
            cnv = ucnv_open(name, &errorCode);
            જો(U_SUCCESS(errorCode) && cnv != nullptr) {
                name = ucnv_getName(cnv, &errorCode);
            }
        }

        જો(name == nullptr || name[0] == 0
            || U_FAILURE(errorCode) || cnv == nullptr
            || uprv_strlen(name)>=કદ(gDefaultConverterNameBuffer))
        {
            /* Panic time, let's use a fallback. */
#જો (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#અથવાજો U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_SWAP_LFNL_OPTION_STRING;
#નહીંતર
            name = "ibm-37_P100-1995";
#endif
        }

        internalSetName(name, &errorCode);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    પાછા name;
#endif
}

#જો U_CHARSET_IS_UTF8
U_CAPI શૂન્ય U_EXPORT2 ucnv_setDefaultName(અચલ અક્ષર *) {}
#નહીંતર
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI શૂન્ય U_EXPORT2
ucnv_setDefaultName(અચલ અક્ષર *converterName) {
    જો(converterName==nullptr) {
        /* reset to the default codepage */
        gDefaultConverterName=nullptr;
    } નહીંતર {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;
        અચલ અક્ષર *name = nullptr;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &errorCode);
        જો(U_SUCCESS(errorCode) && cnv != nullptr) {
            name = ucnv_getName(cnv, &errorCode);
        }

        જો(U_SUCCESS(errorCode) && name!=nullptr) {
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

#જો !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_swap(અચલ UDataSwapper *ds,
          અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
          UErrorCode *pErrorCode) {
    અચલ UDataInfo *pInfo;
    int32_t headerSize;

    અચલ uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t offset, count, staticDataSize;
    int32_t size;

    અચલ UConverterStaticData *inStaticData;
    UConverterStaticData *outStaticData;

    અચલ _MBCSHeader *inMBCSHeader;
    _MBCSHeader *outMBCSHeader;
    _MBCSHeader mbcsHeader;
    uint32_t mbcsHeaderLength;
    UBool noFromU=ખોટું;

    uint8_t outputType;

    int32_t maxFastUChar, mbcsIndexLength;

    અચલ int32_t *inExtIndexes;
    int32_t extOffset;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }

    /* check data format and format version */
    pInfo=(અચલ UDataInfo *)((અચલ અક્ષર *)inData+4);
    જો(!(
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
        પાછા 0;
    }

    inBytes=(અચલ uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    inStaticData=(અચલ UConverterStaticData *)inBytes;
    outStaticData=(UConverterStaticData *)outBytes;

    જો(length<0) {
        staticDataSize=ds->readUInt32(inStaticData->structSize);
    } નહીંતર {
        length-=headerSize;
        જો( length<(int32_t)કદ(UConverterStaticData) ||
            (uint32_t)length<(staticDataSize=ds->readUInt32(inStaticData->structSize))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
    }

    જો(length>=0) {
        /* swap the static data */
        જો(inStaticData!=outStaticData) {
            uprv_memcpy(outStaticData, inStaticData, staticDataSize);
        }

        ds->swapArray32(ds, &inStaticData->structSize, 4,
                           &outStaticData->structSize, pErrorCode);
        ds->swapArray32(ds, &inStaticData->codepage, 4,
                           &outStaticData->codepage, pErrorCode);

        ds->swapInvChars(ds, inStaticData->name, (int32_t)uprv_strlen(inStaticData->name),
                            outStaticData->name, pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            પાછા 0;
        }
    }

    inBytes+=staticDataSize;
    જો (outBytes != nullptr) outBytes+=staticDataSize;
    જો(length>=0) {
        length-=(int32_t)staticDataSize;
    }

    /* check for supported conversionType values */
    જો(inStaticData->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        inMBCSHeader=(અચલ _MBCSHeader *)inBytes;
        outMBCSHeader=(_MBCSHeader *)outBytes;

        જો(0<=length && length<(int32_t)કદ(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
        જો(inMBCSHeader->version[0]==4 && inMBCSHeader->version[1]>=1) {
            mbcsHeaderLength=MBCS_HEADER_V4_LENGTH;
        } નહીંતર જો(inMBCSHeader->version[0]==5 && inMBCSHeader->version[1]>=3 &&
                  ((mbcsHeader.options=ds->readUInt32(inMBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_MASK)==0
        ) {
            mbcsHeaderLength=mbcsHeader.options&MBCS_OPT_LENGTH_MASK;
            noFromU = (mbcsHeader.options & MBCS_OPT_NO_FROM_U) != 0;
        } નહીંતર {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             inMBCSHeader->version[0], inMBCSHeader->version[1]);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            પાછા 0;
        }

        uprv_memcpy(mbcsHeader.version, inMBCSHeader->version, 4);
        mbcsHeader.ગણતરી_States=         ds->readUInt32(inMBCSHeader->ગણતરી_States);
        mbcsHeader.ગણતરી_To_UFallbacks=   ds->readUInt32(inMBCSHeader->ગણતરી_To_UFallbacks);
        mbcsHeader.offsetToUCodeUnits=  ds->readUInt32(inMBCSHeader->offsetToUCodeUnits);
        mbcsHeader.offsetFromUTable=    ds->readUInt32(inMBCSHeader->offsetFromUTable);
        mbcsHeader.offsetFromUBytes=    ds->readUInt32(inMBCSHeader->offsetFromUBytes);
        mbcsHeader.flags=               ds->readUInt32(inMBCSHeader->flags);
        mbcsHeader.fromUBytesLength=    ds->readUInt32(inMBCSHeader->fromUBytesLength);
        /* mbcsHeader.options have been read above */

        extOffset=(int32_t)(mbcsHeader.flags>>8);
        outputType=(uint8_t)mbcsHeader.flags;
        જો(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *pErrorCode=U_UNSUPPORTED_ERROR;
            પાછા 0;
        }

        /* make sure that the output type is known */
        પસંદગી(outputType) {
        સ્થિતિ MBCS_OUTPUT_1:
        સ્થિતિ MBCS_OUTPUT_2:
        સ્થિતિ MBCS_OUTPUT_3:
        સ્થિતિ MBCS_OUTPUT_4:
        સ્થિતિ MBCS_OUTPUT_3_EUC:
        સ્થિતિ MBCS_OUTPUT_4_EUC:
        સ્થિતિ MBCS_OUTPUT_2_SISO:
        સ્થિતિ MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            તોડો;
        મૂળભૂત:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            પાછા 0;
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
        જો( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcsHeader.version[1]>=3 && (maxFastUChar=mbcsHeader.version[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcsIndexLength=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        જો(extOffset==0) {
            size=(int32_t)(mbcsHeader.offsetFromUBytes+mbcsIndexLength);
            જો(!noFromU) {
                size+=(int32_t)mbcsHeader.fromUBytesLength;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            inExtIndexes=nullptr;
        } નહીંતર {
            /* there is extension data after the base data, see ucnv_ext.h */
            જો(length>=0 && length<(extOffset+UCNV_EXT_INDEXES_MIN_LENGTH*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                પાછા 0;
            }

            inExtIndexes=(અચલ int32_t *)(inBytes+extOffset);
            size=extOffset+udata_readInt32(ds, inExtIndexes[UCNV_EXT_SIZE]);
        }

        જો(length>=0) {
            જો(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                પાછા 0;
            }

            /* copy the data for inaccessible bytes */
            જો(inBytes!=outBytes) {
                uprv_memcpy(outBytes, inBytes, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcsHeaderLength*4;
            ds->swapArray32(ds, &inMBCSHeader->ગણતરી_States, count-4,
                               &outMBCSHeader->ગણતરી_States, pErrorCode);

            જો(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                અચલ અક્ષર *inBaseName=(અચલ અક્ષર *)inBytes+count;
                અક્ષર *outBaseName=(અક્ષર *)outBytes+count;
                ds->swapInvChars(ds, inBaseName, (int32_t)uprv_strlen(inBaseName),
                                    outBaseName, pErrorCode);
            } નહીંતર {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                offset=count;
                count=mbcsHeader.ગણતરી_States*1024;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the toUFallbacks[] */
                offset+=count;
                count=mbcsHeader.ગણતરી_To_UFallbacks*8;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the unicodeCodeUnits[] */
                offset=mbcsHeader.offsetToUCodeUnits;
                count=mbcsHeader.offsetFromUTable-offset;
                ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* offset to the stage 1 table, independent of the outputType */
                offset=mbcsHeader.offsetFromUTable;

                જો(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcsHeader.offsetFromUBytes-offset)+mbcsHeader.fromUBytesLength;
                    ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                       outBytes+offset, pErrorCode);
                } નહીંતર {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    જો(inStaticData->unicodeMask&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } નહીંતર {
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
                    પસંદગી(outputType) {
                    સ્થિતિ MBCS_OUTPUT_2:
                    સ્થિતિ MBCS_OUTPUT_3_EUC:
                    સ્થિતિ MBCS_OUTPUT_2_SISO:
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        તોડો;
                    સ્થિતિ MBCS_OUTPUT_4:
                        ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        તોડો;
                    મૂળભૂત:
                        /* just uint8_t[], nothing to swap */
                        તોડો;
                    }

                    જો(mbcsIndexLength!=0) {
                        offset+=count;
                        count=mbcsIndexLength;
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                    }
                }
            }

            જો(extOffset!=0) {
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
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_કિંમતો_INDEX]);
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
    } નહીંતર {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         inStaticData->conversionType);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        પાછા 0;
    }

    પાછા headerSize+(int32_t)staticDataSize+size;
}

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#endif
