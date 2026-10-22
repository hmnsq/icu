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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "utracimp.h"
#অন্তর্ভাব "ucnv_io.h"
#অন্তর্ভাব "ucnv_bld.h"
#অন্তর্ভাব "ucnvmbcs.h"
#অন্তর্ভাব "ucnv_ext.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "ucnv_imp.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "ustr_cnv.h"


#যদি 0
#অন্তর্ভাব <stdio.h>
বাহ্যিক শূন্য UCNV_DEBUG_LOG(অক্ষর *what, অক্ষর *who, শূন্য *p, পূর্ণসংখ্যা l);
#define UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#অন্যথা
# define UCNV_DEBUG_LOG(x,y,z)
#endif

স্থির ধ্রুবক UConverter_Shared_তথ্য * ধ্রুবক
converter_তথ্য[UCNV_সংখ্যা_OF_SUPPORTED_CONVERTER_TYPES]={
    nullptr, nullptr,

#যদি UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#অন্যথা
    &_MBCSData,
#endif

    &Latin1_তথ্য,
    &UTF8_তথ্য, &_UTF16BEData, &_UTF16LEData,
#যদি UCONFIG_ONLY_HTML_CONVERSION
    nullptr, nullptr,
#অন্যথা
    &_UTF32BEData, &_UTF32LEData,
#endif
    nullptr,

#যদি UCONFIG_NO_LEGACY_CONVERSION
    nullptr,
#অন্যথা
    &ISO2022_তথ্য,
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
    nullptr, nullptr, &UTF16_তথ্য, nullptr, nullptr, nullptr,
#অন্যথা
    &UTF7_তথ্য, &Bocu1_তথ্য, &UTF16_তথ্য, &UTF32_তথ্য, &CESU8_তথ্য, &_IMAPData,
#endif

#যদি UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    nullptr,
#অন্যথা
    &Compound_Text_তথ্য
#endif
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
স্থির কাঠামো {
  ধ্রুবক অক্ষর *name;
  ধ্রুবক UConverterType type;
} ধ্রুবক cnvNameType[] = {
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
স্থির UHashtable *SHARED_তথ্য_HASHTABLE = nullptr;
স্থির icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

স্থির ধ্রুবক অক্ষর **gAvailableConverters = nullptr;
স্থির uint16_t g_Available_Converter_গুনতি = 0;
স্থির icu::UInitOnce gAvailableConvertersInitOnce {};

#যদি !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
স্থির অক্ষর g_Default_Converter_Name_বাফার[UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য + 1]; /* +1 for nullptr */
স্থির ধ্রুবক অক্ষর *gDefaultConverterName = nullptr;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
স্থির ধ্রুবক UConverter_Shared_তথ্য *g_Default_Algorithmic_Shared_তথ্য = nullptr;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
স্থির UBool gDefaultConverterContainsOption;

#endif  /* !U_CHARSET_IS_UTF8 */

স্থির ধ্রুবক অক্ষর তথ্য_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
স্থির শূন্য
ucnv_flushAvailableConverterCache() {
    g_Available_Converter_গুনতি = 0;
    যদি (gAvailableConverters) {
        uprv_free(const_cast<অক্ষর**>(gAvailableConverters));
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
    যদি (SHARED_তথ্য_HASHTABLE != nullptr && uhash_গুনতি(SHARED_তথ্য_HASHTABLE) == 0) {
        uhash_close(SHARED_তথ্য_HASHTABLE);
        SHARED_তথ্য_HASHTABLE = nullptr;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flushAvailableConverterCache();

#যদি !U_CHARSET_IS_UTF8
    gDefaultConverterName = nullptr;
    g_Default_Converter_Name_বাফার[0] = 0;
    gDefaultConverterContainsOption = মিথ্যা;
    g_Default_Algorithmic_Shared_তথ্য = nullptr;
#endif

    ফেরত (SHARED_তথ্য_HASHTABLE == nullptr);
}

U_CAPI শূন্য U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_registerCleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

স্থির UBool U_CALLCONV
isCnvAcceptable(শূন্য * /*context*/,
                ধ্রুবক অক্ষর * /*type*/, ধ্রুবক অক্ষর * /*name*/,
                ধ্রুবক UDataInfo *pInfo) {
    ফেরত
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->তথ্য_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->তথ্য_Format[1]==0x6e &&
        pInfo->তথ্য_Format[2]==0x76 &&
        pInfo->তথ্য_Format[3]==0x74 &&
        pInfo->formatVersion[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
স্থির UConverter_Shared_তথ্য*
ucnv_তথ্য_un_Flatten_Clone(UConverterLoadArgs *pArgs, UDataMemory *p_তথ্য, UErrorCode *status)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ধ্রুবক uint8_t* raw = static_cast<ধ্রুবক uint8_t*>(udata_getMemory(p_তথ্য));
    ধ্রুবক UConverter_Static_তথ্য* source = reinterpret_cast<ধ্রুবক UConverter_Static_তথ্য*>(raw);
    UConverter_Shared_তথ্য *তথ্য;
    UConverterType type = static_cast<UConverterType>(source->conversionType);

    যদি(U_FAILURE(*status))
        ফেরত nullptr;

    যদি (static_cast<uint16_t>(type) >= UCNV_সংখ্যা_OF_SUPPORTED_CONVERTER_TYPES ||
        converter_তথ্য[type] == nullptr ||
        !converter_তথ্য[type]->isReferenceCounted ||
        converter_তথ্য[type]->reference_গণক != 1 ||
        source->struct_আকার != আকার(UConverter_Static_তথ্য))
    {
        *status = U_INVALID_TABLE_FORMAT;
        ফেরত nullptr;
    }

    তথ্য = static_cast<UConverter_Shared_তথ্য*>(uprv_malloc(আকার(UConverter_Shared_তথ্য)));
    যদি(তথ্য == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(তথ্য, converter_তথ্য[type], আকার(UConverter_Shared_তথ্য));

    তথ্য->static_তথ্য = source;

    তথ্য->shared_তথ্য_Cached = মিথ্যা;

    /* fill in fields from the loaded data */
    তথ্য->তথ্য_Memory = (শূন্য*)p_তথ্য; /* for future use */

    যদি(তথ্য->impl->load != nullptr) {
        তথ্য->impl->load(তথ্য, pArgs, raw + source->struct_আকার, status);
        যদি(U_FAILURE(*status)) {
            uprv_free(তথ্য);
            ফেরত nullptr;
        }
    }
    ফেরত তথ্য;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
স্থির UConverter_Shared_তথ্য *createConverterFromFile(UConverterLoadArgs *pArgs, UErrorCode * err)
{
    UDataMemory *তথ্য;
    UConverter_Shared_তথ্য *shared_তথ্য;

    UTRACE_ENTRY_OC(UTRACE_UCNV_LOAD);

    যদি (U_FAILURE (*err)) {
        UTRACE_EXIT_STATUS(*err);
        ফেরত nullptr;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", pArgs->name, pArgs->pkg);

    তথ্য = udata_openChoice(pArgs->pkg, তথ্য_TYPE, pArgs->name, isCnvAcceptable, nullptr, err);
    যদি(U_FAILURE(*err))
    {
        UTRACE_EXIT_STATUS(*err);
        ফেরত nullptr;
    }

    shared_তথ্য = ucnv_তথ্য_un_Flatten_Clone(pArgs, তথ্য, err);
    যদি(U_FAILURE(*err))
    {
        udata_close(তথ্য);
        UTRACE_EXIT_STATUS(*err);
        ফেরত nullptr;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_STATUS(shared_তথ্য, *err);
    ফেরত shared_তথ্য;
}

/*returns a converter type from a string
 */
স্থির ধ্রুবক UConverter_Shared_তথ্য *
getAlgorithmicTypeFromName(ধ্রুবক অক্ষর *realName)
{
    uint32_t mid, start, limit;
    uint32_t lastMid;
    পূর্ণসংখ্যা ফলাফল;
    অক্ষর strippedName[UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    start = 0;
    limit = UPRV_LENGTHOF(cnvNameType);
    mid = limit;
    lastMid = UINT32_MAX;

    জন্য (;;) {
        mid = (start + limit) / 2;
        যদি (lastMid == mid) {   /* Have we moved? */
            বিরতি;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        ফলাফল = uprv_strcmp(strippedName, cnvNameType[mid].name);

        যদি (ফলাফল < 0) {
            limit = mid;
        } অন্যথা যদি (ফলাফল > 0) {
            start = mid;
        } অন্যথা {
            ফেরত converter_তথ্য[cnvNameType[mid].type];
        }
    }

    ফেরত nullptr;
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
ucnv_share_Converter_তথ্য(UConverter_Shared_তথ্য * তথ্য)
{
    UErrorCode err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    যদি (SHARED_তথ্য_HASHTABLE == nullptr)
    {
        SHARED_তথ্য_HASHTABLE = uhash_open_আকার(uhash_hashChars, uhash_compareChars, nullptr,
                            ucnv_io_গুনতি_Known_Converters(&err)*UCNV_CACHE_LOAD_FACTOR,
                            &err);
        ucnv_enableCleanup();

        যদি (U_FAILURE(err))
            ফেরত;
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
    তথ্য->shared_তথ্য_Cached = সত্য;

    uhash_put(SHARED_তথ্য_HASHTABLE,
            (শূন্য*) তথ্য->static_তথ্য->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            তথ্য,
            &err);
    UCNV_DEBUG_LOG("put", তথ্য->static_তথ্য->name,তথ্য);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
স্থির UConverter_Shared_তথ্য *
ucnv_get_Shared_Converter_তথ্য(ধ্রুবক অক্ষর *name)
{
    /*special case when no Table has yet been created we return nullptr */
    যদি (SHARED_তথ্য_HASHTABLE == nullptr)
    {
        ফেরত nullptr;
    }
    অন্যথা
    {
        UConverter_Shared_তথ্য *rc;

        rc = static_cast<UConverter_Shared_তথ্য*>(uhash_get(SHARED_তথ্য_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        ফেরত rc;
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
ucnv_delete_Shared_Converter_তথ্য(UConverter_Shared_তথ্য * dead_Shared_তথ্য)
{
    UTRACE_ENTRY_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_তথ্য->static_তথ্য->name, dead_Shared_তথ্য);

    যদি (dead_Shared_তথ্য->reference_গণক > 0) {
        UTRACE_EXIT_মান((int32_t)মিথ্যা);
        ফেরত মিথ্যা;
    }

    যদি (dead_Shared_তথ্য->impl->unload != nullptr) {
        dead_Shared_তথ্য->impl->unload(dead_Shared_তথ্য);
    }

    যদি(dead_Shared_তথ্য->তথ্য_Memory != nullptr)
    {
        UDataMemory *তথ্য = (UDataMemory*)dead_Shared_তথ্য->তথ্য_Memory;
        udata_close(তথ্য);
    }

    uprv_free(dead_Shared_তথ্য);

    UTRACE_EXIT_মান((int32_t)সত্য);
    ফেরত সত্য;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_তথ্য *
ucnv_load(UConverterLoadArgs *pArgs, UErrorCode *err) {
    UConverter_Shared_তথ্য *my_Shared_Converter_তথ্য;

    যদি(err == nullptr || U_FAILURE(*err)) {
        ফেরত nullptr;
    }

    যদি(pArgs->pkg != nullptr && *pArgs->pkg != 0) {
        /* application-provided converters are not currently cached */
        ফেরত createConverterFromFile(pArgs, err);
    }

    my_Shared_Converter_তথ্য = ucnv_get_Shared_Converter_তথ্য(pArgs->name);
    যদি (my_Shared_Converter_তথ্য == nullptr)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_তথ্য = createConverterFromFile(pArgs, err);
        যদি (U_FAILURE (*err) || (my_Shared_Converter_তথ্য == nullptr))
        {
            ফেরত nullptr;
        }
        অন্যথা যদি (!pArgs->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_তথ্য(my_Shared_Converter_তথ্য);
        }
    }
    অন্যথা
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_তথ্য->reference_গণক++;
    }

    ফেরত my_Shared_Converter_তথ্য;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI শূন্য
ucnv_unload(UConverter_Shared_তথ্য *shared_তথ্য) {
    যদি(shared_তথ্য != nullptr) {
        যদি (shared_তথ্য->reference_গণক > 0) {
            shared_তথ্য->reference_গণক--;
        }

        যদি((shared_তথ্য->reference_গণক <= 0)&&(shared_তথ্য->shared_তথ্য_Cached == মিথ্যা)) {
            ucnv_delete_Shared_Converter_তথ্য(shared_তথ্য);
        }
    }
}

U_CFUNC শূন্য
ucnv_unload_Shared_তথ্য_If_Ready(UConverter_Shared_তথ্য *shared_তথ্য)
{
    যদি(shared_তথ্য != nullptr && shared_তথ্য->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        ucnv_unload(shared_তথ্য);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC শূন্য
ucnv_increment_Ref_গুনতি(UConverter_Shared_তথ্য *shared_তথ্য)
{
    যদি(shared_তথ্য != nullptr && shared_তথ্য->isReferenceCounted) {
        umtx_lock(&cnvCacheMutex);
        shared_তথ্য->reference_গণক++;
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
parseConverterOptions(ধ্রুবক অক্ষর *inName,
                      UConverterNamePieces *pPieces,
                      UConverterLoadArgs *pArgs,
                      UErrorCode *err)
{
    অক্ষর *cnvName = pPieces->cnvName;
    অক্ষর c;
    int32_t len = 0;

    pArgs->name=inName;
    pArgs->locale=pPieces->locale;
    pArgs->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    যখন((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        যদি (++len>=UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য) {
            *err = U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            ফেরত;
        }
        *cnvName++=c;
        inName++;
    }
    *cnvName=0;
    pArgs->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    যখন((c=*inName)!=0) {
        যদি(c==UCNV_OPTION_SEP_CHAR) {
            ++inName;
        }

        /* inName is behind an option separator */
        যদি(uprv_strncmp(inName, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            অক্ষর *dest=pPieces->locale;

            /* copy the locale option value */
            inName+=7;
            len=0;
            যখন((c=*inName)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++inName;

                যদি(++len>=ULOC_FULLNAME_CAPACITY) {
                    *err=U_ILLEGAL_ARGUMENT_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    ফেরত;
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
                ফেরত;
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
            যখন(((c = *inName++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            যদি(c==0) {
                ফেরত;
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
U_CFUNC UConverter_Shared_তথ্য *
ucnv_load_Shared_তথ্য(ধ্রুবক অক্ষর *converterName,
                    UConverterNamePieces *pPieces,
                    UConverterLoadArgs *pArgs,
                    UErrorCode * err) {
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args;
    UConverter_Shared_তথ্য *my_Shared_Converter_তথ্য = nullptr;
    UErrorCode internalErrorCode = U_ZERO_ERROR;
    UBool mayContainOption = সত্য;
    UBool checkForAlgorithmic = সত্য;

    যদি (U_FAILURE (*err)) {
        ফেরত nullptr;
    }

    যদি(pPieces == nullptr) {
        যদি(pArgs != nullptr) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_INTERNAL_PROGRAM_ERROR;
            ফেরত nullptr;
        }
        pPieces = &স্তূপ_Pieces;
    }
    যদি(pArgs == nullptr) {
        uprv_memset(&স্তূপ_Args, 0, আকার(স্তূপ_Args));
        স্তূপ_Args.size = (int32_t)আকার(স্তূপ_Args);
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
        ফেরত (UConverter_Shared_তথ্য *)converter_তথ্য[UCNV_UTF8];
#অন্যথা
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        pArgs->name = ucnv_getDefaultName();
        যদি (pArgs->name == nullptr) {
            *err = U_MISSING_RESOURCE_ERROR;
            ফেরত nullptr;
        }
        my_Shared_Converter_তথ্য = (UConverter_Shared_তথ্য *)g_Default_Algorithmic_Shared_তথ্য;
        checkForAlgorithmic = মিথ্যা;
        mayContainOption = gDefaultConverterContainsOption;
        /* the default converter name is already canonical */
#endif
    }
    অন্যথা যদি(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        pArgs->name = "UTF-8";
        ফেরত (UConverter_Shared_তথ্য *)converter_তথ্য[UCNV_UTF8];
    }
    অন্যথা {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, pArgs, err);
        যদি (U_FAILURE(*err)) {
            /* Very bad name used. */
            ফেরত nullptr;
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
        my_Shared_Converter_তথ্য = (UConverter_Shared_তথ্য *)getAlgorithmicTypeFromName(pArgs->name);
    }
    যদি (my_Shared_Converter_তথ্য == nullptr)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        pArgs->nestedLoads=1;
        pArgs->pkg=nullptr;

        umtx_lock(&cnvCacheMutex);
        my_Shared_Converter_তথ্য = ucnv_load(pArgs, err);
        umtx_unlock(&cnvCacheMutex);
        যদি (U_FAILURE (*err) || (my_Shared_Converter_তথ্য == nullptr))
        {
            ফেরত nullptr;
        }
    }

    ফেরত my_Shared_Converter_তথ্য;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ধ্রুবক অক্ষর* converterName,
                     UErrorCode* err)
{
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverter_Shared_তথ্য *my_Shared_Converter_তথ্য;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    যদি(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_তথ্য = ucnv_load_Shared_তথ্য(converterName, &স্তূপ_Pieces, &স্তূপ_Args, err);

        myUConverter = ucnv_create_Converter_From_Shared_তথ্য(
            myUConverter, my_Shared_Converter_তথ্য,
            &স্তূপ_Args,
            err);

        যদি(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
            ফেরত myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_STATUS(*err);
    ফেরত nullptr;
}

U_CFUNC UBool
ucnv_canCreateConverter(ধ্রুবক অক্ষর *converterName, UErrorCode *err) {
    UConverter myUConverter;
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;
    UConverter_Shared_তথ্য *my_Shared_Converter_তথ্য;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN);

    যদি(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        স্তূপ_Args.onlyTestIsLoadable=সত্য;
        my_Shared_Converter_তথ্য = ucnv_load_Shared_তথ্য(converterName, &স্তূপ_Pieces, &স্তূপ_Args, err);
        ucnv_create_Converter_From_Shared_তথ্য(
            &myUConverter, my_Shared_Converter_তথ্য,
            &স্তূপ_Args,
            err);
        ucnv_unload_Shared_তথ্য_If_Ready(my_Shared_Converter_তথ্য);
    }

    UTRACE_EXIT_STATUS(*err);
    ফেরত U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ধ্রুবক অক্ষর* locale,
                                uint32_t options,
                                UErrorCode* err) {
    UConverter *cnv;
    ধ্রুবক UConverter_Shared_তথ্য *shared_তথ্য;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    যদি(type<0 || UCNV_সংখ্যা_OF_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        ফেরত nullptr;
    }

    shared_তথ্য = converter_তথ্য[type];
    যদি(shared_তথ্য == nullptr || shared_তথ্য->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        UTRACE_EXIT_STATUS(U_ILLEGAL_ARGUMENT_ERROR);
        ফেরত nullptr;
    }

    স্তূপ_Args.name = "";
    স্তূপ_Args.options = options;
    স্তূপ_Args.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_তথ্য(
            myUConverter, const_cast<UConverter_Shared_তথ্য*>(shared_তথ্য),
            &স্তূপ_Args, err);

    UTRACE_EXIT_PTR_STATUS(cnv, *err);
    ফেরত cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ধ্রুবক অক্ষর *packageName, ধ্রুবক অক্ষর *converterName, UErrorCode * err)
{
    UConverter *myUConverter;
    UConverter_Shared_তথ্য *my_Shared_Converter_তথ্য;
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;

    UTRACE_ENTRY_OC(UTRACE_UCNV_OPEN_PACKAGE);

    যদি(U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        ফেরত nullptr;
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
        ফেরত nullptr;
    }
    স্তূপ_Args.nestedLoads=1;
    স্তূপ_Args.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_তথ্য = createConverterFromFile(&স্তূপ_Args, err);

    যদি (U_FAILURE(*err)) {
        UTRACE_EXIT_STATUS(*err);
        ফেরত nullptr;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_তথ্য(nullptr, my_Shared_Converter_তথ্য, &স্তূপ_Args, err);

    যদি (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_STATUS(*err);
        ফেরত nullptr;
    }

    UTRACE_EXIT_PTR_STATUS(myUConverter, *err);
    ফেরত myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_তথ্য(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_তথ্য* my_Shared_Converter_তথ্য,
                                   UConverterLoadArgs* pArgs,
                                   UErrorCode* err)
{
    UBool isCopyLocal;

    যদি(U_FAILURE(*err)) {
        ucnv_unload_Shared_তথ্য_If_Ready(my_Shared_Converter_তথ্য);
        ফেরত myUConverter;
    }
    যদি(myUConverter == nullptr)
    {
        myUConverter = (UConverter *) uprv_malloc (আকার (UConverter));
        যদি(myUConverter == nullptr)
        {
            *err = U_MEMORY_ALLOCATION_ERROR;
            ucnv_unload_Shared_তথ্য_If_Ready(my_Shared_Converter_তথ্য);
            ফেরত nullptr;
        }
        isCopyLocal = মিথ্যা;
    } অন্যথা {
        isCopyLocal = সত্য;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, আকার(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_তথ্য = my_Shared_Converter_তথ্য;
    myUConverter->options = pArgs->options;
    যদি(!pArgs->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_TO_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->toUnicodeStatus = my_Shared_Converter_তথ্য->toUnicodeStatus;
        myUConverter->maxBytesPerUChar = my_Shared_Converter_তথ্য->static_তথ্য->maxBytesPerChar;
        myUConverter->subChar1 = my_Shared_Converter_তথ্য->static_তথ্য->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_তথ্য->static_তথ্য->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_তথ্য->static_তথ্য->subChar, myUConverter->subCharLen);
        myUConverter->toUCallbackReason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    যদি(my_Shared_Converter_তথ্য->impl->open != nullptr) {
        my_Shared_Converter_তথ্য->impl->open(myUConverter, pArgs, err);
        যদি(U_FAILURE(*err) && !pArgs->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            ফেরত nullptr;
        }
    }

    ফেরত myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_তথ্য *my_Shared_তথ্য = nullptr;
    int32_t pos;
    int32_t table_Deleted_অঙ্ক = 0;
    ধ্রুবক UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, remaining;

    UTRACE_ENTRY_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    যদি (SHARED_তথ্য_HASHTABLE == nullptr) {
        UTRACE_EXIT_মান((int32_t)0);
        ফেরত 0;
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
    করো {
        remaining = 0;
        pos = UHASH_FIRST;
        যখন ((e = uhash_পরবর্তী_Element (SHARED_তথ্য_HASHTABLE, &pos)) != nullptr)
        {
            my_Shared_তথ্য = (UConverter_Shared_তথ্য *) e->value.pointer;
            /*deletes only if reference counter == 0 */
            যদি (my_Shared_তথ্য->reference_গণক == 0)
            {
                table_Deleted_অঙ্ক++;

                UCNV_DEBUG_LOG("del",my_Shared_তথ্য->static_তথ্য->name,my_Shared_তথ্য);

                uhash_removeElement(SHARED_তথ্য_HASHTABLE, e);
                my_Shared_তথ্য->shared_তথ্য_Cached = মিথ্যা;
                ucnv_delete_Shared_Converter_তথ্য (my_Shared_তথ্য);
            } অন্যথা {
                ++remaining;
            }
        }
    } যখন(++i == 1 && remaining > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", remaining);

    UTRACE_EXIT_মান(table_Deleted_অঙ্ক);
    ফেরত table_Deleted_অঙ্ক;
}

/* available converters list --------------------------------------------------- */

স্থির শূন্য U_CALLCONV initAvailableConvertersList(UErrorCode &errCode) {
    U_ASSERT(g_Available_Converter_গুনতি == 0);
    U_ASSERT(gAvailableConverters == nullptr);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_openAllNames(&errCode);
    int32_t all_Converter_গুনতি = uenum_গুনতি(allConvEnum, &errCode);
    যদি (U_FAILURE(errCode)) {
        ফেরত;
    }

    /* We can't have more than "*converterTable" converters to open */
    gAvailableConverters = static_cast<ধ্রুবক অক্ষর**>(uprv_malloc(all_Converter_গুনতি * আকার(অক্ষর*)));
    যদি (!gAvailableConverters) {
        errCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UErrorCode localStatus = U_ZERO_ERROR;
    UConverter অস্থায়ী_Converter;
    ucnv_close(ucnv_createConverter(&অস্থায়ী_Converter, nullptr, &localStatus));

    g_Available_Converter_গুনতি = 0;

    জন্য (int32_t idx = 0; idx < all_Converter_গুনতি; idx++) {
        localStatus = U_ZERO_ERROR;
        ধ্রুবক অক্ষর *converterName = uenum_পরবর্তী(allConvEnum, nullptr, &localStatus);
        যদি (ucnv_canCreateConverter(converterName, &localStatus)) {
            gAvailableConverters[g_Available_Converter_গুনতি++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


স্থির UBool haveAvailableConverterList(UErrorCode *pErrorCode) {
    umtx_initOnce(gAvailableConvertersInitOnce, &initAvailableConvertersList, *pErrorCode);
    ফেরত U_SUCCESS(*pErrorCode);
}

U_CFUNC uint16_t
ucnv_bld_গুনতি_Available_Converters(UErrorCode *pErrorCode) {
    যদি (haveAvailableConverterList(pErrorCode)) {
        ফেরত g_Available_Converter_গুনতি;
    }
    ফেরত 0;
}

U_CFUNC ধ্রুবক অক্ষর *
ucnv_bld_getAvailableConverter(uint16_t n, UErrorCode *pErrorCode) {
    যদি (haveAvailableConverterList(pErrorCode)) {
        যদি (n < g_Available_Converter_গুনতি) {
            ফেরত gAvailableConverters[n];
        }
        *pErrorCode = U_সূচক_OUTOFBOUNDS_ERROR;
    }
    ফেরত nullptr;
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
স্থির অন্তঃরেখ শূন্য
internalSetName(ধ্রুবক অক্ষর *name, UErrorCode *status) {
    UConverterNamePieces স্তূপ_Pieces;
    UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool containsOption = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != nullptr);
    ধ্রুবক UConverter_Shared_তথ্য *algorithmic_Shared_তথ্য;

    স্তূপ_Args.name = name;
    যদি(containsOption) {
        স্তূপ_Pieces.cnvName[0] = 0;
        স্তূপ_Pieces.locale[0] = 0;
        স্তূপ_Pieces.options = 0;
        parseConverterOptions(name, &স্তূপ_Pieces, &স্তূপ_Args, status);
        যদি(U_FAILURE(*status)) {
            ফেরত;
        }
    }
    algorithmic_Shared_তথ্য = getAlgorithmicTypeFromName(স্তূপ_Args.name);

    umtx_lock(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_তথ্য = algorithmic_Shared_তথ্য;
    gDefaultConverterContainsOption = containsOption;
    uprv_memcpy(g_Default_Converter_Name_বাফার, name, length);
    g_Default_Converter_Name_বাফার[length]=0;

    /* gDefaultConverterName MUST be the last global var set by this function.  */
    /*    It is the variable checked in ucnv_getDefaultName() to see if initialization is required. */
    //    But there is nothing here preventing that from being reordered, either by the compiler
    //             or hardware. I'm adding the mutex to ucnv_getDefaultName for now. UMTX_CHECK is not enough.
    //             -- Andy
    gDefaultConverterName = g_Default_Converter_Name_বাফার;

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

U_CAPI ধ্রুবক অক্ষর*  U_EXPORT2
ucnv_getDefaultName() {
#যদি U_CHARSET_IS_UTF8
    ফেরত "UTF-8";
#অন্যথা
    /* local variable to be thread-safe */
    ধ্রুবক অক্ষর *name;

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
            || uprv_strlen(name)>=আকার(g_Default_Converter_Name_বাফার))
        {
            /* Panic time, let's use a fallback. */
#যদি (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#নতুবা_যদি U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_SWAP_LFNL_OPTION_STRING;
#অন্যথা
            name = "ibm-37_P100-1995";
#endif
        }

        internalSetName(name, &errorCode);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    ফেরত name;
#endif
}

#যদি U_CHARSET_IS_UTF8
U_CAPI শূন্য U_EXPORT2 ucnv_setDefaultName(ধ্রুবক অক্ষর *) {}
#অন্যথা
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI শূন্য U_EXPORT2
ucnv_setDefaultName(ধ্রুবক অক্ষর *converterName) {
    যদি(converterName==nullptr) {
        /* reset to the default codepage */
        gDefaultConverterName=nullptr;
    } অন্যথা {
        UErrorCode errorCode = U_ZERO_ERROR;
        UConverter *cnv = nullptr;
        ধ্রুবক অক্ষর *name = nullptr;

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
ucnv_swap(ধ্রুবক UDataSwapper *ds,
          ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
          UErrorCode *pErrorCode) {
    ধ্রুবক UDataInfo *pInfo;
    int32_t header_আকার;

    ধ্রুবক uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t offset, count, static_তথ্য_আকার;
    int32_t size;

    ধ্রুবক UConverter_Static_তথ্য *in_Static_তথ্য;
    UConverter_Static_তথ্য *out_Static_তথ্য;

    ধ্রুবক _MBCSHeader *inMBCSHeader;
    _MBCSHeader *outMBCSHeader;
    _MBCSHeader mbcsHeader;
    uint32_t mbcs_Header_দৈর্ঘ্য;
    UBool noFromU=মিথ্যা;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_সূচক_দৈর্ঘ্য;

    ধ্রুবক int32_t *inExtIndexes;
    int32_t extOffset;

    /* udata_swapDataHeader checks the arguments */
    header_আকার=udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য+4);
    যদি(!(
        pInfo->তথ্য_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->তথ্য_Format[1]==0x6e &&
        pInfo->তথ্য_Format[2]==0x76 &&
        pInfo->তথ্য_Format[3]==0x74 &&
        pInfo->formatVersion[0]==6 &&
        pInfo->formatVersion[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->তথ্য_Format[0], pInfo->তথ্য_Format[1],
                         pInfo->তথ্য_Format[2], pInfo->তথ্য_Format[3],
                         pInfo->formatVersion[0], pInfo->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    inBytes=(ধ্রুবক uint8_t *)in_তথ্য+header_আকার;
    outBytes=(out_তথ্য == nullptr) ? nullptr : (uint8_t *)out_তথ্য+header_আকার;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    in_Static_তথ্য=(ধ্রুবক UConverter_Static_তথ্য *)inBytes;
    out_Static_তথ্য=(UConverter_Static_তথ্য *)outBytes;

    যদি(length<0) {
        static_তথ্য_আকার=ds->readUInt32(in_Static_তথ্য->struct_আকার);
    } অন্যথা {
        length-=header_আকার;
        যদি( length<(int32_t)আকার(UConverter_Static_তথ্য) ||
            (uint32_t)length<(static_তথ্য_আকার=ds->readUInt32(in_Static_তথ্য->struct_আকার))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
    }

    যদি(length>=0) {
        /* swap the static data */
        যদি(in_Static_তথ্য!=out_Static_তথ্য) {
            uprv_memcpy(out_Static_তথ্য, in_Static_তথ্য, static_তথ্য_আকার);
        }

        ds->swapArray32(ds, &in_Static_তথ্য->struct_আকার, 4,
                           &out_Static_তথ্য->struct_আকার, pErrorCode);
        ds->swapArray32(ds, &in_Static_তথ্য->codepage, 4,
                           &out_Static_তথ্য->codepage, pErrorCode);

        ds->swapInvChars(ds, in_Static_তথ্য->name, (int32_t)uprv_strlen(in_Static_তথ্য->name),
                            out_Static_তথ্য->name, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            ফেরত 0;
        }
    }

    inBytes+=static_তথ্য_আকার;
    যদি (outBytes != nullptr) outBytes+=static_তথ্য_আকার;
    যদি(length>=0) {
        length-=(int32_t)static_তথ্য_আকার;
    }

    /* check for supported conversionType values */
    যদি(in_Static_তথ্য->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        inMBCSHeader=(ধ্রুবক _MBCSHeader *)inBytes;
        outMBCSHeader=(_MBCSHeader *)outBytes;

        যদি(0<=length && length<(int32_t)আকার(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
        যদি(inMBCSHeader->version[0]==4 && inMBCSHeader->version[1]>=1) {
            mbcs_Header_দৈর্ঘ্য=MBCS_HEADER_V4_দৈর্ঘ্য;
        } অন্যথা যদি(inMBCSHeader->version[0]==5 && inMBCSHeader->version[1]>=3 &&
                  ((mbcsHeader.options=ds->readUInt32(inMBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_MASK)==0
        ) {
            mbcs_Header_দৈর্ঘ্য=mbcsHeader.options&MBCS_OPT_দৈর্ঘ্য_MASK;
            noFromU = (mbcsHeader.options & MBCS_OPT_NO_FROM_U) != 0;
        } অন্যথা {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             inMBCSHeader->version[0], inMBCSHeader->version[1]);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ফেরত 0;
        }

        uprv_memcpy(mbcsHeader.version, inMBCSHeader->version, 4);
        mbcsHeader.গুনতি_States=         ds->readUInt32(inMBCSHeader->গুনতি_States);
        mbcsHeader.গুনতি_To_UFallbacks=   ds->readUInt32(inMBCSHeader->গুনতি_To_UFallbacks);
        mbcsHeader.offsetToUCodeUnits=  ds->readUInt32(inMBCSHeader->offsetToUCodeUnits);
        mbcsHeader.offsetFromUTable=    ds->readUInt32(inMBCSHeader->offsetFromUTable);
        mbcsHeader.offsetFromUBytes=    ds->readUInt32(inMBCSHeader->offsetFromUBytes);
        mbcsHeader.flags=               ds->readUInt32(inMBCSHeader->flags);
        mbcsHeader.from_UBytes_দৈর্ঘ্য=    ds->readUInt32(inMBCSHeader->from_UBytes_দৈর্ঘ্য);
        /* mbcsHeader.options have been read above */

        extOffset=(int32_t)(mbcsHeader.flags>>8);
        outputType=(uint8_t)mbcsHeader.flags;
        যদি(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ফেরত 0;
        }

        /* make sure that the output type is known */
        নির্বাচন(outputType) {
        ক্ষেত্র MBCS_OUTPUT_1:
        ক্ষেত্র MBCS_OUTPUT_2:
        ক্ষেত্র MBCS_OUTPUT_3:
        ক্ষেত্র MBCS_OUTPUT_4:
        ক্ষেত্র MBCS_OUTPUT_3_EUC:
        ক্ষেত্র MBCS_OUTPUT_4_EUC:
        ক্ষেত্র MBCS_OUTPUT_2_SISO:
        ক্ষেত্র MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            বিরতি;
        পূর্বনির্ধারিত:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *pErrorCode=U_UNSUPPORTED_ERROR;
            ফেরত 0;
        }

        /* calculate the length of the MBCS data */

        /*
         * utf8Friendly MBCS files (mbcsHeader.version 4.3)
         * contain an additional mbcsIndex table:
         *   uint16_t[(maxFastUChar+1)>>6];
         * where maxFastUChar=((mbcsHeader.version[2]<<8)|0xff).
         */
        maxFastUChar=0;
        mbcs_সূচক_দৈর্ঘ্য=0;
        যদি( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcsHeader.version[1]>=3 && (maxFastUChar=mbcsHeader.version[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_সূচক_দৈর্ঘ্য=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        যদি(extOffset==0) {
            size=(int32_t)(mbcsHeader.offsetFromUBytes+mbcs_সূচক_দৈর্ঘ্য);
            যদি(!noFromU) {
                size+=(int32_t)mbcsHeader.from_UBytes_দৈর্ঘ্য;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            inExtIndexes=nullptr;
        } অন্যথা {
            /* there is extension data after the base data, see ucnv_ext.h */
            যদি(length>=0 && length<(extOffset+UCNV_EXT_INDEXES_MIN_দৈর্ঘ্য*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                ফেরত 0;
            }

            inExtIndexes=(ধ্রুবক int32_t *)(inBytes+extOffset);
            size=extOffset+udata_readInt32(ds, inExtIndexes[UCNV_EXT_আকার]);
        }

        যদি(length>=0) {
            যদি(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                ফেরত 0;
            }

            /* copy the data for inaccessible bytes */
            যদি(inBytes!=outBytes) {
                uprv_memcpy(outBytes, inBytes, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_Header_দৈর্ঘ্য*4;
            ds->swapArray32(ds, &inMBCSHeader->গুনতি_States, count-4,
                               &outMBCSHeader->গুনতি_States, pErrorCode);

            যদি(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ধ্রুবক অক্ষর *inBaseName=(ধ্রুবক অক্ষর *)inBytes+count;
                অক্ষর *outBaseName=(অক্ষর *)outBytes+count;
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
                    count=(mbcsHeader.offsetFromUBytes-offset)+mbcsHeader.from_UBytes_দৈর্ঘ্য;
                    ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                       outBytes+offset, pErrorCode);
                } অন্যথা {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    যদি(in_Static_তথ্য->unicodeMask&UCNV_HAS_SUPPLEMENTARY) {
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
                    count= noFromU ? 0 : mbcsHeader.from_UBytes_দৈর্ঘ্য;
                    নির্বাচন(outputType) {
                    ক্ষেত্র MBCS_OUTPUT_2:
                    ক্ষেত্র MBCS_OUTPUT_3_EUC:
                    ক্ষেত্র MBCS_OUTPUT_2_SISO:
                        ds->swapArray16(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        বিরতি;
                    ক্ষেত্র MBCS_OUTPUT_4:
                        ds->swapArray32(ds, inBytes+offset, (int32_t)count,
                                           outBytes+offset, pErrorCode);
                        বিরতি;
                    পূর্বনির্ধারিত:
                        /* just uint8_t[], nothing to swap */
                        বিরতি;
                    }

                    যদি(mbcs_সূচক_দৈর্ঘ্য!=0) {
                        offset+=count;
                        count=mbcs_সূচক_দৈর্ঘ্য;
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
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_সূচক]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_দৈর্ঘ্য]);
                ds->swapArray32(ds, inBytes+offset, length*4, outBytes+offset, pErrorCode);

                /* swap toUUChars[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_UCHARS_সূচক]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_TO_U_UCHARS_দৈর্ঘ্য]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUTableUChars[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_UCHARS_সূচক]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_দৈর্ঘ্য]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUTableValues[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_মানসমূহ_সূচক]);
                /* same length as for fromUTableUChars[] */
                ds->swapArray32(ds, inBytes+offset, length*4, outBytes+offset, pErrorCode);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_12_সূচক]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_12_দৈর্ঘ্য]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUStage3[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_সূচক]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_দৈর্ঘ্য]);
                ds->swapArray16(ds, inBytes+offset, length*2, outBytes+offset, pErrorCode);

                /* swap fromUStage3b[] */
                offset=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_B_সূচক]);
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_FROM_U_STAGE_3_B_দৈর্ঘ্য]);
                ds->swapArray32(ds, inBytes+offset, length*4, outBytes+offset, pErrorCode);

                /* swap indexes[] */
                length=udata_readInt32(ds, inExtIndexes[UCNV_EXT_INDEXES_দৈর্ঘ্য]);
                ds->swapArray32(ds, inBytes, length*4, outBytes, pErrorCode);
            }
        }
    } অন্যথা {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         in_Static_তথ্য->conversionType);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    ফেরত header_আকার+(int32_t)static_তথ্য_আকার+size;
}

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#endif
