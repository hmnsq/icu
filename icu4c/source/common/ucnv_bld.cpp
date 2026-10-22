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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/putil.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/ucnv.h"
#समावेश "unicode/uloc.h"
#समावेश "mutex.h"
#समावेश "putilimp.h"
#समावेश "uassert.h"
#समावेश "utracimp.h"
#समावेश "ucnv_io.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnvmbcs.h"
#समावेश "ucnv_ext.h"
#समावेश "ucnv_cnv.h"
#समावेश "ucnv_imp.h"
#समावेश "uhash.h"
#समावेश "umutex.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "ucln_cmn.h"
#समावेश "ustr_cnv.h"


#अगर 0
#समावेश <stdio.h>
बाह्य व्योम UCNV_DEBUG_LOG(अक्षर *what, अक्षर *who, व्योम *p, पूर्णांक l);
#घोषणा UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#अन्यथा
# define UCNV_DEBUG_LOG(x,y,z)
#पूर्ण_अगर

अटल स्थिर UConverter_Shared_तथ्य * स्थिर
converter_तथ्य[UCNV_संख्या_OF_SUPPORTED_CONVERTER_TYPES]={
    nullptr, nullptr,

#अगर UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#अन्यथा
    &_MBCSData,
#पूर्ण_अगर

    &Latin1_तथ्य,
    &UTF8_तथ्य, &_UTF16BEData, &_UTF16LEData,
#अगर UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr,
#अन्यथा
    &_UTF32BEData, &_UTF32LEData,
#पूर्ण_अगर
    nullptr,

#अगर UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#अन्यथा
    &ISO2022_तथ्य,
#पूर्ण_अगर

#अगर UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr,
#अन्यथा
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#पूर्ण_अगर

#अगर UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#अन्यथा
    &_SCSUData,
#पूर्ण_अगर


#अगर UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#अन्यथा
    &_ISCIIData,
#पूर्ण_अगर

    &_ASCIIData,
#अगर UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr, &UTF16_तथ्य, nullptr, nullptr, nullptr,
#अन्यथा
    &UTF7_तथ्य, &Bocu1_तथ्य, &UTF16_तथ्य, &UTF32_तथ्य, &CESU8_तथ्य, &_IMAPData,
#पूर्ण_अगर

#अगर UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#अन्यथा
    &Compound_Text_तथ्य
#पूर्ण_अगर
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
अटल काष्ठा {
  स्थिर अक्षर *name;
  स्थिर UConverterType type;
} स्थिर cnvNameType[] = {
#अगर !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#पूर्ण_अगर
#अगर !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#पूर्ण_अगर
#अगर !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#पूर्ण_अगर
#अगर !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#पूर्ण_अगर
#अगर !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#पूर्ण_अगर
  { "iso88591", UCNV_LATIN_1 },
#अगर !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#पूर्ण_अगर
#अगर !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#पूर्ण_अगर
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#अगर U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#अन्यथा
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#पूर्ण_अगर
#अगर !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#अगर U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#अन्यथा
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#पूर्ण_अगर
#पूर्ण_अगर
#अगर !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#पूर्ण_अगर
  { "utf8", UCNV_UTF8 },
#अगर !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_TEXT}
#पूर्ण_अगर
};


/*initializes some global variables */
अटल UHashtable *SHARED_तथ्य_HASHTABLE = nullptr;
अटल icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

अटल स्थिर अक्षर **gAvailableConverters = nullptr;
अटल uint16_t g_Available_Converter_गिनती = 0;
अटल icu::UInitOnce gAvailableConvertersInitOnce {};

#अगर !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
अटल अक्षर g_Default_Converter_Name_बफर[UCNV_MAX_CONVERTER_NAME_लंबाई + 1]; /* +1 for nullptr */
अटल स्थिर अक्षर *gDefaultConverterName = nullptr;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
अटल स्थिर UConverter_Shared_तथ्य *g_Default_Algorithmic_Shared_तथ्य = nullptr;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
अटल UBool gDefaultConverterContainsOption;

#पूर्ण_अगर  /* !U_CHARSET_IS_UTF8 */

अटल स्थिर अक्षर तथ्य_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
अटल व्योम
ucnv_flushAvailableConverterCache() {
    g_Available_Converter_गिनती = 0;
    अगर (gAvailableConverters) {
        uprv_free(const_cast<अक्षर**>(gAvailableConverters));
        gAvailableConverters = nullptr;
    }
    gAvailableConvertersInitOnce.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
अटल UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    अगर (SHARED_तथ्य_HASHTABLE != nullptr && uhash_गिनती(SHARED_तथ्य_HASHTABLE) == 0) {
        uhash_close(SHARED_तथ्य_HASHTABLE);
        SHARED_तथ्य_HASHTABLE = nullptr;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flushAvailableConverterCache();

#अगर !U_CHARSET_IS_UTF8
    gDefaultConverterName = nullptr;
    g_Default_Converter_Name_बफर[0] = 0;
    gDefaultConverterContainsOption = false;
    g_Default_Algorithmic_Shared_तथ्य = nullptr;
#पूर्ण_अगर

    वापस (SHARED_तथ्य_HASHTABLE == nullptr);
}

U_CAPI व्योम U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_registerCleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

अटल UBool U_CALLCONV
isCnvAcceptable(व्योम * /*context*/,
                स्थिर अक्षर * /*type*/, स्थिर अक्षर * /*name*/,
                स्थिर UDataInfo *pInfo) {
    वापस
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->तथ्य_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->तथ्य_Format[1]==0x6e &&
        pInfo->तथ्य_Format[2]==0x76 &&
        pInfo->तथ्य_Format[3]==0x74 &&
        pInfo->formatVersion[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
अटल UConverter_Shared_तथ्य*
ucnv_तथ्य_un_Flatten_Clone(UConverterLoadArgs *pArgs, UDataMemory *p_तथ्य, UErrorCode *status)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    स्थिर uint8_t* raw = static_cast<स्थिर uint8_t*>(udata_getMemory(p_तथ्य));
    स्थिर UConverter_Static_तथ्य* source = reinterpret_cast<स्थिर UConverter_Static_तथ्य*>(raw);
    UConverter_Shared_तथ्य *तथ्य;
    UConverterType type = static_cast<UConverterType>(source->conversionType);

    अगर(U_FAILURE(*status))
        वापस nullptr;

    अगर (static_cast<uint16_t>(type) >= UCNV_संख्या_OF_SUPPORTED_CONVERTER_TYPES ||
        converter_तथ्य[type] == nullptr ||
        !converter_तथ्य[type]->isReferenceCounted ||
        converter_तथ्य[type]->reference_गणक != 1 ||
        source->struct_आकार != माप(UConverter_Static_तथ्य))
    {
        *status = U_INVALID_TABLE_FORMAT;
        वापस nullptr;
    }

    तथ्य = static_cast<UConverter_Shared_तथ्य*>(uprv_malloc(माप(UConverter_Shared_तथ्य)));
    अगर(तथ्य == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(तथ्य, converter_तथ्य[type], माप(UConverter_Shared_तथ्य));

    तथ्य->static_तथ्य = source;

    तथ्य->shared_तथ्य_Cached = false;

    /* fill in fields from the loaded data */
    तथ्य->तथ्य_Memory = (व्योम*)p_तथ्य; /* for future use */

    अगर(तथ्य->impl->load != nullptr) {
        तथ्य->impl->load(तथ्य, pArgs, raw + source->struct_आकार, status);
        अगर(U_FAILURE(*status)) {
            uprv_free(तथ्य);
            वापस nullptr;
        }
    }
    वापस तथ्य;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
अटल UConverter_Shared_तथ्य *createConverterFromFile(UConverterLoadArgs *pArgs, UErrorCode * err)
{
    UDataMemory *तथ्य;
    UConverter_Shared_तथ्य *shared_तथ्य;

    UTRACE_ENTRY_OC(UTRACE_UCNV_LOAD);

    अगर (U_FAILURE (*err)) {
        UTRACE_EXIT_STATUS(*err);
        वापस nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", pArgs->name, pArgs->pkg);

    तथ्य = udata_openChoice(pArgs->pkg, तथ्य_TYPE, pArgs->name, isCnvAcceptable, nullptr, err);
    अगर(U_FAILURE(*err))
    {
        UTRACE_EXIT_STATUS(*err);
        वापस nullptr;
    }

    shared_तथ्य = ucnv_तथ्य_un_Flatten_Clone(pArgs, तथ्य, err);
    अगर(U_FAILURE(*err))
    {
        udata_close(तथ्य);
        UTRACE_EXIT_STATUS(*err);
        वापस nullptr;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_STATUS(shared_तथ्य, *err);
    वापस shared_तथ्य;
}

/*returns a converter type from a string
 */
अटल स्थिर UConverter_Shared_तथ्य *
getAlgorithmicTypeFromName(स्थिर अक्षर *realName)
{
    uint32_t mid, start, limit;
    uint32_t lastMid;
    पूर्णांक परिणाम;
    अक्षर strippedName[UCNV_MAX_CONVERTER_NAME_लंबाई];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    start = 0;
    limit = UPRV_LENGTHOF(cnvNameType);
    mid = limit;
    lastMid = UINT32_MAX;

    क्रम (;;) {
        mid = (start + limit) / 2;
        अगर (lastMid == mid) {   /* Have we moved? */
            अवरोध;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        परिणाम = uprv_strcmp(strippedName, cnvNameType[mid].name);

        अगर (परिणाम < 0) {
            limit = mid;
        } अन्यथा अगर (परिणाम > 0) {
            start = mid;
        } अन्यथा {
            वापस converter_तथ्य[cnvNameType[mid].type];
        }
    }

    वापस nullptr;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#घोषणा UCNV_CACHE_LOAD_FACTOR 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
अटल व्योम
ucnv_share_Converter_तथ्य(UConverter_Shared_तथ्य * तथ्य)
{
    UErrorCode err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    अगर (SHARED_तथ्य_HASHTABLE == nullptr)
    {
        SHARED_तथ्य_HASHTABLE = uhash_open_आकार(uhash_hashChars, uhash_compareChars, nullptr,
                            ucnv_io_गिनती_Known_Converters(&err)*UCNV_CACHE_LOAD_FACTOR,
                            &err);
        ucnv_enableCleanup();

        अगर (U_FAILURE(err))
            वापस;
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
    तथ्य->shared_तथ्य_Cached = true;

    uhash_put(SHARED_तथ्य_HASHTABLE,
            (व्योम*) तथ्य->static_तथ्य->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            तथ्य,
            &err);
    UCNV_DEBUG_LOG("put", तथ्य->static_तथ्य->name,तथ्य);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
अटल UConverter_Shared_तथ्य *
ucnv_get_Shared_Converter_तथ्य(स्थिर अक्षर *name)
{
    /*special case when no Table has yet been created we return nullptr */
    अगर (SHARED_तथ्य_HASHTABLE == nullptr)
    {
        वापस nullptr;
    }
    अन्यथा
    {
        UConverter_Shared_तथ्य *rc;

        rc = static_cast<UConverter_Shared_तथ्य*>(uhash_get(SHARED_तथ्य_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        वापस rc;
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
अटल UBool
ucnv_delete_Shared_Converter_तथ्य(UConverter_Shared_तथ्य * dead_Shared_तथ्य)
{
    UTRACE_ENTRY_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_तथ्य->static_तथ्य->name, dead_Shared_तथ्य);

    अगर (dead_Shared_तथ्य->reference_गणक > 0) {
        UTRACE_EXIT_मूल्य((int32_t)false);
        वापस false;
    }

    अगर (dead_Shared_तथ्य->impl->unload != nullptr) {
        dead_Shared_तथ्य->impl->unload(dead_Shared_तथ्य);
    }

    अगर(dead_Shared_तथ्य->तथ्य_Memory != nullptr)
    {
        UDataMemory *तथ्य = (UDataMemory*)dead_Shared_तथ्य->तथ्य_Memory;
        udata_close(तथ्य);
    }

    uprv_free(dead_Shared_तथ्य);

    UTRACE_EXIT_मूल्य((int32_t)true);
    वापस true;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_तथ्य *
ucnv_load(UConverterLoadArgs *pArgs, UErrorCode *err) {
    UConverter_Shared_तथ्य *my_Shared_Converter_तथ्य;

    अगर(err == nullptr || U_FAILURE(*err)) {
        वापस nullptr;
    }

    अगर(pArgs->pkg != nullptr && *pArgs->pkg != 0) {
        /* application-provided converters are not currently cached */
        वापस createConverterFromFile(pArgs, err);
    }

    my_Shared_Converter_तथ्य = ucnv_get_Shared_Converter_तथ्य(pArgs->name);
    अगर (my_Shared_Converter_तथ्य == nullptr)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_तथ्य = createConverterFromFile(pArgs, err);
        अगर (U_FAILURE (*err) || (my_Shared_Converter_तथ्य == nullptr))
        {
            वापस nullptr;
        }
        अन्यथा अगर (!pArgs->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_तथ्य(my_Shared_Converter_तथ्य);
        }
    }
    अन्यथा
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_तथ्य->reference_गणक++;
    }

    वापस my_Shared_Converter_तथ्य;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI व्योम
ucnv_unload(UConverter_Shared_तथ्य *shared_तथ्य) {
    अगर(shared_तथ्य != nullptr) {
        अगर (shared_तथ्य->reference_गणक > 0) {
            shared_तथ्य->reference_गणक--;
        }

        अगर((shared_तथ्य->reference_गणक <= 0)&&(shared_तथ्य->shared_तथ्य_Cached == false)) {
            ucnv_delete_Shared_Converter_तथ्य(shared_तथ्य);
        }
    }
}

U_CFUNC व्योम
ucnv_unload_Shared_तथ्य_If_Ready(UConverter_Shared_तथ्य *shared_तथ्य)
{
    अगर(shared_तथ्य != nullptr && shared_तथ्य->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        ucnv_unload(shared_तथ्य);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC व्योम
ucnv_increment_Ref_गिनती(UConverter_Shared_तथ्य *shared_तथ्य)
{
    अगर(shared_तथ्य != nullptr && shared_तथ्य->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        shared_तथ्य->reference_गणक++;
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
अटल व्योम
parseConverterOptions(स्थिर अक्षर *inName,
                      UConverterNamePieces *pPieces,
                      UConverterLoadArgs *pArgs,
                      UErrorCode *err)
{
    अक्षर *cnvName = pPieces->cnvName;
    अक्षर c;
    int32_t len = 0;

    pArgs->name=inName;
    pArgs->locale=pPieces->locale;
    pArgs->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    जबतक((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        अगर (++len>=UCNV_MAX_CONVERTER_NAME_लंबाई) {
            *err = U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            वापस;
        }
        *cnvName++=c;
        inName++;
    }
    *cnvName=0;
    pArgs->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    जबतक((c=*inName)!=0) {
        अगर(c==UCNV_OPTION_SEP_CHAR) {
            ++inName;
        }

        /* inName is behind an option separator */
        अगर(uprv_strncmp(inName, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            अक्षर *dest=pPieces->locale;

            /* copy the locale option value */
            inName+=7;
            len=0;
            जबतक((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++inName;

                अगर(++len>=ULOC_FULLNAME_CAPACITY) {
                    *err=U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    वापस;
                }

                *dest++=c;
            }
            *dest=0;
        } अन्यथा अगर(uprv_strncmp(inName, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            inName+=8;
            c=*inName;
            अगर(c==0) {
                pArgs->options=(pPieces->options&=~UCNV_OPTION_VERSION);
                वापस;
            } अन्यथा अगर (static_cast<uint8_t>(c - '0') < 10) {
                pArgs->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_VERSION) | static_cast<uint32_t>(c - '0');
                ++inName;
            }
        } अन्यथा अगर(uprv_strncmp(inName, "swaplfnl", 8)==0) {
            inName+=8;
            pArgs->options=(pPieces->options|=UCNV_OPTION_SWAP_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } अन्यथा {
            /* ignore any other options until we define some */
            जबतक(((c = *inName++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            अगर(c==0) {
                वापस;
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
U_CFUNC UConverter_Shared_तथ्य *
ucnv_load_Shared_तथ्य(स्थिर अक्षर *converterName,
                    UConverterNamePieces *pPieces,
                    UConverterLoadArgs *pArgs,
                    UErrorCode * err) {
    UConverterNamePieces ढेर_Pieces;
    UConverterLoadArgs ढेर_Args;
    UConverter_Shared_तथ्य *my_Shared_Converter_तथ्य = nullptr;
    UErrorCode internalErrorCode = U_ZERO_ERROR;
    UBool mayContainOption = true;
    UBool checkForAlgorithmic = true;

    अगर (U_FAILURE (*err)) {
        वापस nullptr;
    }

    अगर(pPieces == nullptr) {
        अगर(pArgs != nullptr) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_INTERNAL_PROGRAM_ERROR;
            वापस nullptr;
        }
        pPieces = &ढेर_Pieces;
    }
    अगर(pArgs == nullptr) {
        uprv_memset(&ढेर_Args, 0, माप(ढेर_Args));
        ढेर_Args.size = (int32_t)माप(ढेर_Args);
        pArgs = &ढेर_Args;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    pArgs->name = converterName;
    pArgs->locale = pPieces->locale;
    pArgs->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    अगर (converterName == nullptr) {
#अगर U_CHARSET_IS_UTF8
        pArgs->name = "UTF-8";
        वापस (UConverter_Shared_तथ्य *)converter_तथ्य[UCNV_UTF8];
#अन्यथा
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        pArgs->name = ucnv_getDefaultName();
        अगर (pArgs->name == nullptr) {
            *err = U_MISSING_RESOURCE_ERROR;
            वापस nullptr;
        }
        my_Shared_Converter_तथ्य = (UConverter_Shared_तथ्य *)g_Default_Algorithmic_Shared_तथ्य;
        checkForAlgorithmic = false;
        mayContainOption = gDefaultConverterContainsOption;
        /* the default converter name is already canonical */
#पूर्ण_अगर
    }
    अन्यथा अगर(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        pArgs->name = "UTF-8";
        वापस (UConverter_Shared_तथ्य *)converter_तथ्य[UCNV_UTF8];
    }
    अन्यथा {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, pArgs, err);
        अगर (U_FAILURE(*err)) {
            /* Very bad name used. */
            वापस nullptr;
        }

        /* get the canonical converter name */
        pArgs->name = ucnv_io_getConverterName(pArgs->name, &mayContainOption, &internalErrorCode);
        अगर (U_FAILURE(internalErrorCode) || pArgs->name == nullptr) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            pArgs->name = pPieces->cnvName;
        } अन्यथा अगर (internalErrorCode == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    अगर(mayContainOption && pArgs->name != pPieces->cnvName) {
        parseConverterOptions(pArgs->name, pPieces, pArgs, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    अगर (checkForAlgorithmic) {
        my_Shared_Converter_तथ्य = (UConverter_Shared_तथ्य *)getAlgorithmicTypeFromName(pArgs->name);
    }
    अगर (my_Shared_Converter_तथ्य == nullptr)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        pArgs->nestedLoads=1;
        pArgs->pkg=nullptr;

        umtx_lock(&cnvCacheMutex);
        my_Shared_Converter_तथ्य = ucnv_load(pArgs, err);
        umtx_unlock(&cnvCacheMutex);
        अगर (U_FAILURE (*err) || (my_Shared_Converter_तथ्य == nullptr))
        {
            वापस nullptr;
        }
    }

    वापस my_Shared_Converter_तथ्य;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     स्थिर अक्षर* converterName,
                     UErrorCode* err)
{
    UConverterNamePieces ढेर_Pieces;
    UConverterLoadArgs ढेर_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverter_Shared_तथ्य *my_Shared_Converter_तथ्य;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    अगर(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_तथ्य = ucnv_load_Shared_तथ्य(converterName, &ढेर_Pieces, &ढेर_Args, err);

        myUConverter = ucnv_create_Converter_From_Shared_तथ्य(
            myUConverter, my_Shared_Converter_तथ्य,
            &ढेर_Args,
            err);

        अगर(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
            वापस myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_STATUS(*err);
    वापस nullptr;
}

U_CFUNC UBool
ucnv_canCreateConverter(स्थिर अक्षर *converterName, UErrorCode *err) {
    UConverter myUConverter;
    UConverterNamePieces ढेर_Pieces;
    UConverterLoadArgs ढेर_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverter_Shared_तथ्य *my_Shared_Converter_तथ्य;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    अगर(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ढेर_Args.onlyTestIsLoadable=true;
        my_Shared_Converter_तथ्य = ucnv_load_Shared_तथ्य(converterName, &ढेर_Pieces, &ढेर_Args, err);
        ucnv_create_Converter_From_Shared_तथ्य(
            &myUConverter, my_Shared_Converter_तथ्य,
            &ढेर_Args,
            err);
        ucnv_unload_Shared_तथ्य_If_Ready(my_Shared_Converter_तथ्य);
    }

    UTRACE_EXIT_STATUS(*err);
    वापस U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                स्थिर अक्षर* locale,
                                uint32_t options,
                                UErrorCode* err) {
    UConverter *cnv;
    स्थिर UConverter_Shared_तथ्य *shared_तथ्य;
    UConverterLoadArgs ढेर_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    अगर(type<0 || UCNV_संख्या_OF_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        वापस nullptr;
    }

    shared_तथ्य = converter_तथ्य[type];
    अगर(shared_तथ्य == nullptr || shared_तथ्य->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        वापस nullptr;
    }

    ढेर_Args.name = "";
    ढेर_Args.options = options;
    ढेर_Args.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_तथ्य(
            myUConverter, const_cast<UConverter_Shared_तथ्य*>(shared_तथ्य),
            &ढेर_Args, err);

    UTRACE_EXIT_PTR_STATUS(cnv, *err);
    वापस cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(स्थिर अक्षर *packageName, स्थिर अक्षर *converterName, UErrorCode * err)
{
    UConverter *myUConverter;
    UConverter_Shared_तथ्य *my_Shared_Converter_तथ्य;
    UConverterNamePieces ढेर_Pieces;
    UConverterLoadArgs ढेर_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_PACKAGE);

    अगर(U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        वापस nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ढेर_Pieces.cnvName[0] = 0;
    ढेर_Pieces.locale[0] = 0;
    ढेर_Pieces.options = 0;
    parseConverterOptions(converterName, &ढेर_Pieces, &ढेर_Args, err);
    अगर (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_STATUS(*err);
        वापस nullptr;
    }
    ढेर_Args.nestedLoads=1;
    ढेर_Args.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_तथ्य = createConverterFromFile(&ढेर_Args, err);

    अगर (U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        वापस nullptr;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_तथ्य(nullptr, my_Shared_Converter_तथ्य, &ढेर_Args, err);

    अगर (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_STATUS(*err);
        वापस nullptr;
    }

    UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
    वापस myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_तथ्य(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_तथ्य* my_Shared_Converter_तथ्य,
                                   UConverterLoadArgs* pArgs,
                                   UErrorCode* err)
{
    UBool isCopyLocal;

    अगर(U_FAILURE(*err)) {
        ucnv_unload_Shared_तथ्य_If_Ready(my_Shared_Converter_तथ्य);
        वापस myUConverter;
    }
    अगर(myUConverter == nullptr)
    {
        myUConverter = (UConverter *) uprv_malloc (माप (UConverter));
        अगर(myUConverter == nullptr)
        {
            *err = U_MEMORY_ALLOCATION_ERROR;
            ucnv_unload_Shared_तथ्य_If_Ready(my_Shared_Converter_तथ्य);
            वापस nullptr;
        }
        isCopyLocal = false;
    } अन्यथा {
        isCopyLocal = true;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, माप(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_तथ्य = my_Shared_Converter_तथ्य;
    myUConverter->options = pArgs->options;
    अगर(!pArgs->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_TO_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->toUnicodeStatus = my_Shared_Converter_तथ्य->toUnicodeStatus;
        myUConverter->maxBytesPerUChar = my_Shared_Converter_तथ्य->static_तथ्य->maxBytesPerChar;
        myUConverter->subChar1 = my_Shared_Converter_तथ्य->static_तथ्य->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_तथ्य->static_तथ्य->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_तथ्य->static_तथ्य->subChar, myUConverter->subCharLen);
        myUConverter->toUCallbackReason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    अगर(my_Shared_Converter_तथ्य->impl->खोलो != nullptr) {
        my_Shared_Converter_तथ्य->impl->खोलो(myUConverter, pArgs, err);
        अगर(U_FAILURE(*err) && !pArgs->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            वापस nullptr;
        }
    }

    वापस myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_तथ्य *my_Shared_तथ्य = nullptr;
    int32_t pos;
    int32_t table_Deleted_अंक = 0;
    स्थिर UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, remaining;

    UTRACE_ENTRY_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    अगर (SHARED_तथ्य_HASHTABLE == nullptr) {
        UTRACE_EXIT_मूल्य((int32_t)0);
        वापस 0;
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
    करो {
        remaining = 0;
        pos = UHASH_FIRST;
        जबतक ((e = uhash_अग्र_Element (SHARED_तथ्य_HASHTABLE, &pos)) != nullptr)
        {
            my_Shared_तथ्य = (UConverter_Shared_तथ्य *) e->value.pointer;
            /*deletes only if reference counter == 0 */
            अगर (my_Shared_तथ्य->reference_गणक == 0)
            {
                table_Deleted_अंक++;

                UCNV_DEBUG_LOG("del",my_Shared_तथ्य->static_तथ्य->name,my_Shared_तथ्य);

                uhash_removeElement(SHARED_तथ्य_HASHTABLE, e);
                my_Shared_तथ्य->shared_तथ्य_Cached = false;
                ucnv_delete_Shared_Converter_तथ्य (my_Shared_तथ्य);
            } अन्यथा {
                ++remaining;
            }
        }
    } जबतक(++i == 1 && remaining > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", remaining);

    UTRACE_EXIT_मूल्य(table_Deleted_अंक);
    वापस table_Deleted_अंक;
}

/* available converters list --------------------------------------------------- */

अटल व्योम U_CALLCONV initAvailableConvertersList(UErrorCode &errCode) {
    U_ASSERT(g_Available_Converter_गिनती == 0);
    U_ASSERT(gAvailableConverters == nullptr);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_openAllNames(&errCode);
    int32_t all_Converter_गिनती = uenum_गिनती(allConvEnum, &errCode);
    अगर (U_FAILURE(errCode)) {
        वापस;
    }

    /* We can't have more than "*converterTable" converters to open */
    gAvailableConverters = static_cast<स्थिर अक्षर**>(uprv_malloc(all_Converter_गिनती * माप(अक्षर*)));
    अगर (!gAvailableConverters) {
        errCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UErrorCode localStatus = U_ZERO_ERROR;
    UConverter अस्थायी_Converter;
    ucnv_close(ucnv_createConverter(&अस्थायी_Converter, nullptr, &localStatus));

    g_Available_Converter_गिनती = 0;

    क्रम (int32_t idx = 0; idx < all_Converter_गिनती; idx++) {
        localStatus = U_ZERO_ERROR;
        स्थिर अक्षर *converterName = uenum_अग्र(allConvEnum, nullptr, &localStatus);
        अगर (ucnv_canCreateConverter(converterName, &localStatus)) {
            gAvailableConverters[g_Available_Converter_गिनती++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


अटल UBool haveAvailableConverterList(UErrorCode *pErrorCode) {
    umtx_initOnce(gAvailableConvertersInitOnce, &initAvailableConvertersList, *pErrorCode);
    वापस U_SUCCESS(*pErrorCode);
}

U_CFUNC uint16_t
ucnv_bld_गिनती_Available_Converters(UErrorCode *pErrorCode) {
    अगर (haveAvailableConverterList(pErrorCode)) {
        वापस g_Available_Converter_गिनती;
    }
    वापस 0;
}

U_CFUNC स्थिर अक्षर *
ucnv_bld_getAvailableConverter(uint16_t n, UErrorCode *pErrorCode) {
    अगर (haveAvailableConverterList(pErrorCode)) {
        अगर (n < g_Available_Converter_गिनती) {
            वापस gAvailableConverters[n];
        }
        *pErrorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
    }
    वापस nullptr;
}

/* default converter name --------------------------------------------------- */

#अगर !U_CHARSET_IS_UTF8
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
अटल अंतरभूत व्योम
internalSetName(स्थिर अक्षर *name, UErrorCode *status) {
    UConverterNamePieces ढेर_Pieces;
    UConverterLoadArgs ढेर_Args=UCNV_LOAD_ARGS_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool containsOption = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != nullptr);
    स्थिर UConverter_Shared_तथ्य *algorithmic_Shared_तथ्य;

    ढेर_Args.name = name;
    अगर(containsOption) {
        ढेर_Pieces.cnvName[0] = 0;
        ढेर_Pieces.locale[0] = 0;
        ढेर_Pieces.options = 0;
        parseConverterOptions(name, &ढेर_Pieces, &ढेर_Args, status);
        अगर(U_FAILURE(*status)) {
            वापस;
        }
    }
    algorithmic_Shared_तथ्य = getAlgorithmicTypeFromName(ढेर_Args.name);

    umtx_lock(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_तथ्य = algorithmic_Shared_तथ्य;
    gDefaultConverterContainsOption = containsOption;
    uprv_memcpy(g_Default_Converter_Name_बफर, name, length);
    g_Default_Converter_Name_बफर[length]=0;

    /* gDefaultConverterName MUST be the last global var set by this function.  */
    /*    It is the variable checked in ucnv_getDefaultName() to see if initialization is required. */
    //    But there is nothing here preventing that from being reordered, either by the compiler
    //             or hardware. I'm adding the mutex to ucnv_getDefaultName for now. UMTX_CHECK is not enough.
    //             -- Andy
    gDefaultConverterName = g_Default_Converter_Name_बफर;

    ucnv_enableCleanup();

    umtx_unlock(&cnvCacheMutex);
}
#पूर्ण_अगर

/*
 * In order to be really thread-safe, the get function would have to take
 * a buffer parameter and copy the current string inside a mutex block.
 * This implementation only tries to be really thread-safe while
 * setting the name.
 * It assumes that setting a pointer is atomic.
 */

U_CAPI स्थिर अक्षर*  U_EXPORT2
ucnv_getDefaultName() {
#अगर U_CHARSET_IS_UTF8
    वापस "UTF-8";
#अन्यथा
    /* local variable to be thread-safe */
    स्थिर अक्षर *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex lock(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    अगर(name==nullptr) {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        अगर(name != nullptr) {
            cnv = ucnv_open(name, &errorCode);
            अगर(U_SUCCESS(errorCode) && cnv != nullptr) {
                name = ucnv_getName(cnv, &errorCode);
            }
        }

        अगर(name == nullptr || name[0] == 0
            || U_FAILURE(errorCode) || cnv == nullptr
            || uprv_strlen(name)>=माप(g_Default_Converter_Name_बफर))
        {
            /* Panic time, let's use a fallback. */
#अगर (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#या_अगर U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_SWAP_LFNL_OPTION_STRING;
#अन्यथा
            name = "ibm-37_P100-1995";
#पूर्ण_अगर
        }

        internalSetName(name, &errorCode);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    वापस name;
#पूर्ण_अगर
}

#अगर U_CHARSET_IS_UTF8
U_CAPI व्योम U_EXPORT2 ucnv_setDefaultName(स्थिर अक्षर *) {}
#अन्यथा
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI व्योम U_EXPORT2
ucnv_setDefaultName(स्थिर अक्षर *converterName) {
    अगर(converterName==nullptr) {
        /* reset to the default codepage */
        gDefaultConverterName=nullptr;
    } अन्यथा {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;
        स्थिर अक्षर *name = nullptr;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &errorCode);
        अगर(U_SUCCESS(errorCode) && cnv != nullptr) {
            name = ucnv_getName(cnv, &errorCode);
        }

        अगर(U_SUCCESS(errorCode) && name!=nullptr) {
            internalSetName(name, &errorCode);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#पूर्ण_अगर

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#अगर !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_swap(स्थिर UDataSwapper *ds,
          स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
          UErrorCode *pErrorCode) {
    स्थिर UDataInfo *pInfo;
    int32_t header_आकार;

    स्थिर uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t offset, count, static_तथ्य_आकार;
    int32_t size;

    स्थिर UConverter_Static_तथ्य *in_Static_तथ्य;
    UConverter_Static_तथ्य *out_Static_तथ्य;

    स्थिर _MBCSHeader *inMBCSHeader;
    _MBCSHeader *outMBCSHeader;
    _MBCSHeader mbcsHeader;
    uint32_t mbcs_Header_लंबाई;
    UBool noFromU=false;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_सूचकांक_लंबाई;

    स्थिर int32_t *inExtIndexes;
    int32_t extOffset;

    /* udata_swapDataHeader checks the arguments */
    header_आकार=udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* check data format and format version */
    pInfo=(स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
    अगर(!(
        pInfo->तथ्य_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->तथ्य_Format[1]==0x6e &&
        pInfo->तथ्य_Format[2]==0x76 &&
        pInfo->तथ्य_Format[3]==0x74 &&
        pInfo->formatVersion[0]==6 &&
        pInfo->formatVersion[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1],
                         pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3],
                         pInfo->formatVersion[0], pInfo->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    inBytes=(स्थिर uint8_t *)in_तथ्य+header_आकार;
    outBytes=(out_तथ्य == nullptr) ? nullptr : (uint8_t *)out_तथ्य+header_आकार;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    in_Static_तथ्य=(स्थिर UConverter_Static_तथ्य *)inBytes;
    out_Static_तथ्य=(UConverter_Static_तथ्य *)outBytes;

    अगर(length<0) {
        static_तथ्य_आकार=ds->readUInt32(in_Static_तथ्य->struct_आकार);
    } अन्यथा {
        length-=header_आकार;
        अगर( length<(int32_t)माप(UConverter_Static_तथ्य) ||
            (uint32_t)length<(static_तथ्य_आकार=ds->readUInt32(in_Static_तथ्य->struct_आकार))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
    }

    अगर(length>=0) {
        /* swap the static data */
        अगर(in_Static_तथ्य!=out_Static_तथ्य) {
            uprv_memcpy(out_Static_तथ्य, in_Static_तथ्य, static_तथ्य_आकार);
        }

        ds->swapArray32(ds, &in_Static_तथ्य->struct_आकार, 4,
                           &out_Static_तथ्य->struct_आकार, pErrorCode);
        ds->swapArray32(ds, &in_Static_तथ्य->codepage, 4,
                           &out_Static_तथ्य->codepage, pErrorCode);

        ds->swapInvChars(ds, in_Static_तथ्य->name, (int32_t)uprv_strlen(in_Static_तथ्य->name),
                            out_Static_तथ्य->name, pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            वापस 0;
        }
    }

    inBytes+=static_तथ्य_आकार;
    अगर (outBytes != nullptr) outBytes+=static_तथ्य_आकार;
    अगर(length>=0) {
        length-=(int32_t)static_तथ्य_आकार;
    }

    /* check for supported conversionType values */
    अगर(in_Static_तथ्य->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        inMBCSHeader=(स्थिर _MBCSHeader *)inBytes;
        outMBCSHeader=(_MBCSHeader *)outBytes;

        अगर(0<=length && length<(int32_t)माप(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
        अगर(inMBCSHeader->version[0]==4 && inMBCSHeader->version[1]>=1) {
            mbcs_Header_लंबाई=MBCS_HEADER_V4_लंबाई;
        } अन्यथा अगर(inMBCSHeader->version[0]==5 && inMBCSHeader->version[1]>=3 &&
                  ((mbcsHeader.options=ds->readUInt32(inMBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_MASK)==0
        ) {
            mbcs_Header_लंबाई=mbcsHeader.options&MBCS_OPT_लंबाई_MASK;
            noFromU = (mbcsHeader.options & MBCS_OPT_NO_FROM_U) != 0;
        } अन्यथा {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             inMBCSHeader->version[0], inMBCSHeader->version[1]);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            वापस 0;
        }

        uprv_memcpy(mbcsHeader.version, inMBCSHeader->version, 4);
        mbcsHeader.गिनती_States=         ds->readUInt32(inMBCSHeader->गिनती_States);
        mbcsHeader.गिनती_To_UFallbacks=   ds->readUInt32(inMBCSHeader->गिनती_To_UFallbacks);
        mbcsHeader.offsetToUCodeUnits=  ds->readUInt32(inMBCSHeader->offsetToUCodeUnits);
        mbcsHeader.offsetFromUTable=    ds->readUInt32(inMBCSHeader->offsetFromUTable);
        mbcsHeader.offsetFromUBytes=    ds->readUInt32(inMBCSHeader->offsetFromUBytes);
        mbcsHeader.flags=               ds->readUInt32(inMBCSHeader->flags);
        mbcsHeader.from_UBytes_लंबाई=    ds->readUInt32(inMBCSHeader->from_UBytes_लंबाई);
        /* mbcsHeader.options have been read above */

        extOffset=(int32_t)(mbcsHeader.flags>>8);
        outputType=(uint8_t)mbcsHeader.flags;
        अगर(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *pErrorCode=U_UNSUPPORTED_ERROR;
            वापस 0;
        }

        /* make sure that the output type is known */
        चयन(outputType) {
        हाल MBCS_OUTPUT_1:
        हाल MBCS_OUTPUT_2:
        हाल MBCS_OUTPUT_3:
        हाल MBCS_OUTPUT_4:
        हाल MBCS_OUTPUT_3_EUC:
        हाल MBCS_OUTPUT_4_EUC:
        हाल MBCS_OUTPUT_2_SISO:
        हाल MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            अवरोध;
        शेष:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            वापस 0;
        }

        /* calculate the length of the MBCS data */

        /*
         * utf8Friendly MBCS files (mbcsHeader.version 4.3)
         * contain an additional mbcsIndex table:
         *   uint16_t[(maxFastUChar+1)>>6];
         * where maxFastUChar=((mbcsHeader.version[2]<<8)|0xff).
         */
        maxFastUChar=0;
        mbcs_सूचकांक_लंबाई=0;
        अगर( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcsHeader.version[1]>=3 && (maxFastUChar=mbcsHeader.version[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_सूचकांक_लंबाई=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        अगर(extOffset==0) {
            size=(int32_t)(mbcsHeader.offsetFromUBytes+mbcs_सूचकांक_लंबाई);
            अगर(!noFromU) {
                size+=(int32_t)mbcsHeader.from_UBytes_लंबाई;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            inExtIndexes=nullptr;
        } अन्यथा {
            /* there is extension data after the base data, see ucnv_ext.h */
            अगर(length>=0 && length<(extOffset+UCNV_EXT_INDEXES_MIN_लंबाई*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                वापस 0;
            }

            inExtIndexes=(स्थिर int32_t *)(inBytes+extOffset);
            size=extOffset+udata_readInt32(ds, inExtIndexes[UCNV_EXT_आकार]);
        }

        अगर(length>=0) {
            अगर(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                वापस 0;
            }

            /* copy the data for inaccessible bytes */
            अगर(inBytes!=outBytes) {
                uprv_memcpy(outBytes, inBytes, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_Header_लंबाई*4;
            ds->swapArray32(ds, &inMBCSHeader->गिनती_States, count-4,
                               &outMBCSHeader->गिनती_States, pErrorCode);

            अगर(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                स्थिर अक्षर *inBaseName=(स्थिर अक्षर *)inBytes+count;
                अक्षर *outBaseName=(अक्षर *)outBytes+count;
                ds->swapInvChars(ds, inBaseName, (int32_t)uprv_strlen(inBaseName),
                                    outBaseName, pErrorCode);
            } अन्यथा {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                offset=count;
                count=mbcsHeader.गिनती_States*1024;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the toUFallbacks[] */
                offset+=count;
                count=mbcsHeader.गिनती_To_UFallbacks*8;
                ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* swap the unicodeCodeUnits[] */
                offset=mbcsHeader.offsetToUCodeUnits;
                count=mbcsHeader.offsetFromUTable-offset;
                ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                   outBytes+offset, pErrorCode);

                /* offset to the stage 1 table, independent of the outputType */
                offset=mbcsHeader.offsetFromUTable;

                अगर(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcsHeader.offsetFromUBytes-offset)+mbcsHeader.from_UBytes_लंबाई;
                    ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                       outBytes+offset, pErrorCode);
                } अन्यथा {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    अगर(in_Static_तथ्य->unicodeMask&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } अन्यथा {
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
                    count= noFromU ? 0 : mbcsHeader.from_UBytes_लंबाई;
                    चयन(outputType) {
                    हाल MBCS_OUTPUT_2:
                    हाल MBCS_OUTPUT_3_EUC:
                    हाल MBCS_OUTPUT_2_SISO:
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        अवरोध;
                    हाल MBCS_OUTPUT_4:
                        ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        अवरोध;
                    शेष:
                        /* just uint8_t[], nothing to swap */
                        अवरोध;
                    }

                    अगर(mbcs_सूचकांक_लंबाई!=0) {
                        offset+=count;
                        count=mbcs_सूचकांक_लंबाई;
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                    }
                }
            }

            अगर(extOffset!=0) {
                /* swap the extension data */
                inBytes+=extOffset;
                outBytes+=extOffset;

                /* swap toUTable[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_सूचकांक]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_लंबाई]);
                ds->swapArray32(ds, inBytes+offset, length*4, outBytes+offset, pErrorCode);

                /* swap toUUChars[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_UCHARS_सूचकांक]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_UCHARS_लंबाई]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUTableUChars[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_UCHARS_सूचकांक]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_लंबाई]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUTableValues[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_मूल्यावली_सूचकांक]);
                /* same length as for fromUTableUChars[] */
                ds->swapArray32(ds, inBytes+offset, length*4, outBytes+offset, pErrorCode);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_12_सूचकांक]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_12_लंबाई]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUStage3[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_सूचकांक]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_लंबाई]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUStage3b[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_B_सूचकांक]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_B_लंबाई]);
                ds->swapArray32(ds, inBytes+offset, length*4, outBytes+offset, pErrorCode);

                /* swap indexes[] */
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_INDEXES_लंबाई]);
                ds->swapArray32(ds, inBytes, length*4, outBytes, pErrorCode);
            }
        }
    } अन्यथा {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         in_Static_तथ्य->conversionType);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    वापस header_आकार+(int32_t)static_तथ्य_आकार+size;
}

#पूर्ण_अगर /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#पूर्ण_अगर
