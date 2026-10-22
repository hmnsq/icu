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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "utracimp.h"
#అన్తర్భావ "ucnv_io.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnvmbcs.h"
#అన్తర్భావ "ucnv_ext.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "ucnv_imp.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "ustr_cnv.h"


#ఒకవేళ 0
#అన్తర్భావ <stdio.h>
బాహ్య శూన్య UCNV_DEBUG_LOG(అక్షరం *what, అక్షరం *who, శూన్య *p, పూర్ణాంకం l);
#నిర్వచించు UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#లేకపోతే
# define UCNV_DEBUG_LOG(x,y,z)
#అయితే_ముగింపు

స్థిర స్థిరరాశి UConverter_Shared_దత్తాంశం * స్థిరరాశి
converter_దత్తాంశం[UCNV_సంఖ్య_యొక్క_SUPPORTED_CONVERTER_TYPES]={
    శూన్యసూచిక, శూన్యసూచిక,

#ఒకవేళ UCONFIG_NO_LEGACY_CONVERSION
    శూన్యసూచిక,
#లేకపోతే
    &_MBCSData,
#అయితే_ముగింపు

    &Latin1_దత్తాంశం,
    &UTF8_దత్తాంశం, &_UTF16BEData, &_UTF16LEData,
#ఒకవేళ UCONFIG_ONLY_HTML_CONVERSION
    శూన్యసూచిక, శూన్యసూచిక,
#లేకపోతే
    &_UTF32BEData, &_UTF32LEData,
#అయితే_ముగింపు
    శూన్యసూచిక,

#ఒకవేళ UCONFIG_NO_LEGACY_CONVERSION
    శూన్యసూచిక,
#లేకపోతే
    &ISO2022_దత్తాంశం,
#అయితే_ముగింపు

#ఒకవేళ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక,
    శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక,
    శూన్యసూచిక,
#లేకపోతే
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#అయితే_ముగింపు

#ఒకవేళ UCONFIG_ONLY_HTML_CONVERSION
    శూన్యసూచిక,
#లేకపోతే
    &_SCSUData,
#అయితే_ముగింపు


#ఒకవేళ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    శూన్యసూచిక,
#లేకపోతే
    &_ISCIIData,
#అయితే_ముగింపు

    &_ASCIIData,
#ఒకవేళ UCONFIG_ONLY_HTML_CONVERSION
    శూన్యసూచిక, శూన్యసూచిక, &UTF16_దత్తాంశం, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక,
#లేకపోతే
    &UTF7_దత్తాంశం, &Bocu1_దత్తాంశం, &UTF16_దత్తాంశం, &UTF32_దత్తాంశం, &CESU8_దత్తాంశం, &_IMAPData,
#అయితే_ముగింపు

#ఒకవేళ UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    శూన్యసూచిక,
#లేకపోతే
    &Compound_పాఠ్యం_దత్తాంశం
#అయితే_ముగింపు
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
స్థిర నిర్మాణం {
  స్థిరరాశి అక్షరం *name;
  స్థిరరాశి UConverterType type;
} స్థిరరాశి cnvNameType[] = {
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#అయితే_ముగింపు
#ఒకవేళ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#అయితే_ముగింపు
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#అయితే_ముగింపు
#ఒకవేళ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#అయితే_ముగింపు
#ఒకవేళ !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#అయితే_ముగింపు
  { "iso88591", UCNV_LATIN_1 },
#ఒకవేళ !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#అయితే_ముగింపు
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#అయితే_ముగింపు
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#ఒకవేళ U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#లేకపోతే
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#అయితే_ముగింపు
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#ఒకవేళ U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#లేకపోతే
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#అయితే_ముగింపు
#అయితే_ముగింపు
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#అయితే_ముగింపు
  { "utf8", UCNV_UTF8 },
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_పాఠ్యం}
#అయితే_ముగింపు
};


/*initializes some global variables */
స్థిర UHashtable *SHARED_దత్తాంశం_HASHTABLE = శూన్యసూచిక;
స్థిర icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

స్థిర స్థిరరాశి అక్షరం **g_అందుబాటు_Converters = శూన్యసూచిక;
స్థిర uint16_t g_అందుబాటు_Converter_లెక్క = 0;
స్థిర icu::UInitOnce g_అందుబాటు_Converters_Init_Once {};

#ఒకవేళ !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
స్థిర అక్షరం g_Default_Converter_Name_బఫరు[UCNV_MAX_CONVERTER_NAME_నిడివి + 1]; /* +1 for nullptr */
స్థిర స్థిరరాశి అక్షరం *gDefaultConverterName = శూన్యసూచిక;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
స్థిర స్థిరరాశి UConverter_Shared_దత్తాంశం *g_Default_Algorithmic_Shared_దత్తాంశం = శూన్యసూచిక;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
స్థిర UBool g_Default_Converter_కలిగియుంది_Option;

#అయితే_ముగింపు  /* !U_CHARSET_IS_UTF8 */

స్థిర స్థిరరాశి అక్షరం దత్తాంశం_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
స్థిర శూన్య
ucnv_flush_అందుబాటు_Converter_Cache() {
    g_అందుబాటు_Converter_లెక్క = 0;
    ఒకవేళ (g_అందుబాటు_Converters) {
        uprv_free(const_cast<అక్షరం**>(g_అందుబాటు_Converters));
        g_అందుబాటు_Converters = శూన్యసూచిక;
    }
    g_అందుబాటు_Converters_Init_Once.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
స్థిర UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    ఒకవేళ (SHARED_దత్తాంశం_HASHTABLE != శూన్యసూచిక && uhash_లెక్క(SHARED_దత్తాంశం_HASHTABLE) == 0) {
        uhash_close(SHARED_దత్తాంశం_HASHTABLE);
        SHARED_దత్తాంశం_HASHTABLE = శూన్యసూచిక;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flush_అందుబాటు_Converter_Cache();

#ఒకవేళ !U_CHARSET_IS_UTF8
    gDefaultConverterName = శూన్యసూచిక;
    g_Default_Converter_Name_బఫరు[0] = 0;
    g_Default_Converter_కలిగియుంది_Option = అబద్ధం;
    g_Default_Algorithmic_Shared_దత్తాంశం = శూన్యసూచిక;
#అయితే_ముగింపు

    తిరిగి (SHARED_దత్తాంశం_HASHTABLE == శూన్యసూచిక);
}

U_CAPI శూన్య U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

స్థిర UBool U_CALLCONV
isCnvAcceptable(శూన్య * /*context*/,
                స్థిరరాశి అక్షరం * /*type*/, స్థిరరాశి అక్షరం * /*name*/,
                స్థిరరాశి UDataInfo *pInfo) {
    తిరిగి
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->దత్తాంశం_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->దత్తాంశం_Format[1]==0x6e &&
        pInfo->దత్తాంశం_Format[2]==0x76 &&
        pInfo->దత్తాంశం_Format[3]==0x74 &&
        pInfo->format_సంస్కరణ[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
స్థిర UConverter_Shared_దత్తాంశం*
ucnv_దత్తాంశం_un_Flatten_Clone(UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UData_జ్ఞాపకం *p_దత్తాంశం, UError_సంకేతం *స్థితి_2)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    స్థిరరాశి uint8_t* raw = static_cast<స్థిరరాశి uint8_t*>(udata_get_జ్ఞాపకం(p_దత్తాంశం));
    స్థిరరాశి UConverter_Static_దత్తాంశం* ఆధారం = reinterpret_cast<స్థిరరాశి UConverter_Static_దత్తాంశం*>(raw);
    UConverter_Shared_దత్తాంశం *దత్తాంశం;
    UConverterType type = static_cast<UConverterType>(ఆధారం->conversionType);

    ఒకవేళ(U_FAILURE(*స్థితి_2))
        తిరిగి శూన్యసూచిక;

    ఒకవేళ (static_cast<uint16_t>(type) >= UCNV_సంఖ్య_యొక్క_SUPPORTED_CONVERTER_TYPES ||
        converter_దత్తాంశం[type] == శూన్యసూచిక ||
        !converter_దత్తాంశం[type]->isReferenceCounted ||
        converter_దత్తాంశం[type]->referenceCounter != 1 ||
        ఆధారం->struct_కొలత != పరిమాణం(UConverter_Static_దత్తాంశం))
    {
        *స్థితి_2 = U_INVALID_పట్టిక_FORMAT;
        తిరిగి శూన్యసూచిక;
    }

    దత్తాంశం = static_cast<UConverter_Shared_దత్తాంశం*>(uprv_malloc(పరిమాణం(UConverter_Shared_దత్తాంశం)));
    ఒకవేళ(దత్తాంశం == శూన్యసూచిక) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(దత్తాంశం, converter_దత్తాంశం[type], పరిమాణం(UConverter_Shared_దత్తాంశం));

    దత్తాంశం->static_దత్తాంశం = ఆధారం;

    దత్తాంశం->shared_దత్తాంశం_Cached = అబద్ధం;

    /* fill in fields from the loaded data */
    దత్తాంశం->దత్తాంశం_జ్ఞాపకం = (శూన్య*)p_దత్తాంశం; /* for future use */

    ఒకవేళ(దత్తాంశం->impl->load != శూన్యసూచిక) {
        దత్తాంశం->impl->load(దత్తాంశం, p_వాదనలు, raw + ఆధారం->struct_కొలత, స్థితి_2);
        ఒకవేళ(U_FAILURE(*స్థితి_2)) {
            uprv_free(దత్తాంశం);
            తిరిగి శూన్యసూచిక;
        }
    }
    తిరిగి దత్తాంశం;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
స్థిర UConverter_Shared_దత్తాంశం *createConverterFromFile(UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం * err)
{
    UData_జ్ఞాపకం *దత్తాంశం;
    UConverter_Shared_దత్తాంశం *shared_దత్తాంశం;

    UTRACE_నమోదు_OC(UTRACE_UCNV_ఎక్కించు);

    ఒకవేళ (U_FAILURE (*err)) {
        UTRACE_EXIT_స్థితి(*err);
        తిరిగి శూన్యసూచిక;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", p_వాదనలు->name, p_వాదనలు->pkg);

    దత్తాంశం = udata_openChoice(p_వాదనలు->pkg, దత్తాంశం_TYPE, p_వాదనలు->name, isCnvAcceptable, శూన్యసూచిక, err);
    ఒకవేళ(U_FAILURE(*err))
    {
        UTRACE_EXIT_స్థితి(*err);
        తిరిగి శూన్యసూచిక;
    }

    shared_దత్తాంశం = ucnv_దత్తాంశం_un_Flatten_Clone(p_వాదనలు, దత్తాంశం, err);
    ఒకవేళ(U_FAILURE(*err))
    {
        udata_close(దత్తాంశం);
        UTRACE_EXIT_స్థితి(*err);
        తిరిగి శూన్యసూచిక;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_స్థితి(shared_దత్తాంశం, *err);
    తిరిగి shared_దత్తాంశం;
}

/*returns a converter type from a string
 */
స్థిర స్థిరరాశి UConverter_Shared_దత్తాంశం *
getAlgorithmicTypeFromName(స్థిరరాశి అక్షరం *realName)
{
    uint32_t mid, ప్రారంభం, హద్దు;
    uint32_t చివరి_Mid;
    పూర్ణాంకం ఫలితం;
    అక్షరం strippedName[UCNV_MAX_CONVERTER_NAME_నిడివి];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    ప్రారంభం = 0;
    హద్దు = UPRV_LENGTHOF(cnvNameType);
    mid = హద్దు;
    చివరి_Mid = UINT32_MAX;

    కొరకు (;;) {
        mid = (ప్రారంభం + హద్దు) / 2;
        ఒకవేళ (చివరి_Mid == mid) {   /* Have we moved? */
            ఆపు;  /* We haven't moved, and it wasn't found. */
        }
        చివరి_Mid = mid;
        ఫలితం = uprv_strcmp(strippedName, cnvNameType[mid].name);

        ఒకవేళ (ఫలితం < 0) {
            హద్దు = mid;
        } లేకపోతే ఒకవేళ (ఫలితం > 0) {
            ప్రారంభం = mid;
        } లేకపోతే {
            తిరిగి converter_దత్తాంశం[cnvNameType[mid].type];
        }
    }

    తిరిగి శూన్యసూచిక;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#నిర్వచించు UCNV_CACHE_ఎక్కించు_గుణకం 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
స్థిర శూన్య
ucnv_share_Converter_దత్తాంశం(UConverter_Shared_దత్తాంశం * దత్తాంశం)
{
    UError_సంకేతం err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    ఒకవేళ (SHARED_దత్తాంశం_HASHTABLE == శూన్యసూచిక)
    {
        SHARED_దత్తాంశం_HASHTABLE = uhash_open_కొలత(uhash_hashChars, uhash_compareChars, శూన్యసూచిక,
                            ucnv_io_లెక్క_తెలిసిన_Converters(&err)*UCNV_CACHE_ఎక్కించు_గుణకం,
                            &err);
        ucnv_enableCleanup();

        ఒకవేళ (U_FAILURE(err))
            తిరిగి;
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
    దత్తాంశం->shared_దత్తాంశం_Cached = నిజం;

    uhash_put(SHARED_దత్తాంశం_HASHTABLE,
            (శూన్య*) దత్తాంశం->static_దత్తాంశం->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            దత్తాంశం,
            &err);
    UCNV_DEBUG_LOG("put", దత్తాంశం->static_దత్తాంశం->name,దత్తాంశం);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
స్థిర UConverter_Shared_దత్తాంశం *
ucnv_get_Shared_Converter_దత్తాంశం(స్థిరరాశి అక్షరం *name)
{
    /*special case when no Table has yet been created we return nullptr */
    ఒకవేళ (SHARED_దత్తాంశం_HASHTABLE == శూన్యసూచిక)
    {
        తిరిగి శూన్యసూచిక;
    }
    లేకపోతే
    {
        UConverter_Shared_దత్తాంశం *rc;

        rc = static_cast<UConverter_Shared_దత్తాంశం*>(uhash_get(SHARED_దత్తాంశం_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        తిరిగి rc;
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
స్థిర UBool
ucnv_delete_Shared_Converter_దత్తాంశం(UConverter_Shared_దత్తాంశం * dead_Shared_దత్తాంశం)
{
    UTRACE_నమోదు_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_దత్తాంశం->static_దత్తాంశం->name, dead_Shared_దత్తాంశం);

    ఒకవేళ (dead_Shared_దత్తాంశం->referenceCounter > 0) {
        UTRACE_EXIT_విలువ((int32_t)అబద్ధం);
        తిరిగి అబద్ధం;
    }

    ఒకవేళ (dead_Shared_దత్తాంశం->impl->unload != శూన్యసూచిక) {
        dead_Shared_దత్తాంశం->impl->unload(dead_Shared_దత్తాంశం);
    }

    ఒకవేళ(dead_Shared_దత్తాంశం->దత్తాంశం_జ్ఞాపకం != శూన్యసూచిక)
    {
        UData_జ్ఞాపకం *దత్తాంశం = (UData_జ్ఞాపకం*)dead_Shared_దత్తాంశం->దత్తాంశం_జ్ఞాపకం;
        udata_close(దత్తాంశం);
    }

    uprv_free(dead_Shared_దత్తాంశం);

    UTRACE_EXIT_విలువ((int32_t)నిజం);
    తిరిగి నిజం;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_దత్తాంశం *
ucnv_ఎక్కించు(UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం *err) {
    UConverter_Shared_దత్తాంశం *my_Shared_Converter_దత్తాంశం;

    ఒకవేళ(err == శూన్యసూచిక || U_FAILURE(*err)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(p_వాదనలు->pkg != శూన్యసూచిక && *p_వాదనలు->pkg != 0) {
        /* application-provided converters are not currently cached */
        తిరిగి createConverterFromFile(p_వాదనలు, err);
    }

    my_Shared_Converter_దత్తాంశం = ucnv_get_Shared_Converter_దత్తాంశం(p_వాదనలు->name);
    ఒకవేళ (my_Shared_Converter_దత్తాంశం == శూన్యసూచిక)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_దత్తాంశం = createConverterFromFile(p_వాదనలు, err);
        ఒకవేళ (U_FAILURE (*err) || (my_Shared_Converter_దత్తాంశం == శూన్యసూచిక))
        {
            తిరిగి శూన్యసూచిక;
        }
        లేకపోతే ఒకవేళ (!p_వాదనలు->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_దత్తాంశం(my_Shared_Converter_దత్తాంశం);
        }
    }
    లేకపోతే
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_దత్తాంశం->referenceCounter++;
    }

    తిరిగి my_Shared_Converter_దత్తాంశం;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI శూన్య
ucnv_unload(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం) {
    ఒకవేళ(shared_దత్తాంశం != శూన్యసూచిక) {
        ఒకవేళ (shared_దత్తాంశం->referenceCounter > 0) {
            shared_దత్తాంశం->referenceCounter--;
        }

        ఒకవేళ((shared_దత్తాంశం->referenceCounter <= 0)&&(shared_దత్తాంశం->shared_దత్తాంశం_Cached == అబద్ధం)) {
            ucnv_delete_Shared_Converter_దత్తాంశం(shared_దత్తాంశం);
        }
    }
}

U_CFUNC శూన్య
ucnv_unload_Shared_దత్తాంశం_If_Ready(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం)
{
    ఒకవేళ(shared_దత్తాంశం != శూన్యసూచిక && shared_దత్తాంశం->isReferenceCounted) {
        umtx_తాళం(&cnvCacheMutex);
        ucnv_unload(shared_దత్తాంశం);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC శూన్య
ucnv_increment_Ref_లెక్క(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం)
{
    ఒకవేళ(shared_దత్తాంశం != శూన్యసూచిక && shared_దత్తాంశం->isReferenceCounted) {
        umtx_తాళం(&cnvCacheMutex);
        shared_దత్తాంశం->referenceCounter++;
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
స్థిర శూన్య
parseConverterOptions(స్థిరరాశి అక్షరం *లోపల_Name,
                      UConverterNamePieces *pPieces,
                      UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
                      UError_సంకేతం *err)
{
    అక్షరం *cnvName = pPieces->cnvName;
    అక్షరం c;
    int32_t len = 0;

    p_వాదనలు->name=లోపల_Name;
    p_వాదనలు->locale=pPieces->locale;
    p_వాదనలు->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    అయితే((c=*లోపల_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        ఒకవేళ (++len>=UCNV_MAX_CONVERTER_NAME_నిడివి) {
            *err = U_ILLEGAL_వాదం_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            తిరిగి;
        }
        *cnvName++=c;
        లోపల_Name++;
    }
    *cnvName=0;
    p_వాదనలు->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    అయితే((c=*లోపల_Name)!=0) {
        ఒకవేళ(c==UCNV_OPTION_SEP_CHAR) {
            ++లోపల_Name;
        }

        /* inName is behind an option separator */
        ఒకవేళ(uprv_strncmp(లోపల_Name, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            అక్షరం *dest=pPieces->locale;

            /* copy the locale option value */
            లోపల_Name+=7;
            len=0;
            అయితే((c=*లోపల_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++లోపల_Name;

                ఒకవేళ(++len>=ULOC_FULLNAME_సామర్థ్యం) {
                    *err=U_ILLEGAL_వాదం_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    తిరిగి;
                }

                *dest++=c;
            }
            *dest=0;
        } లేకపోతే ఒకవేళ(uprv_strncmp(లోపల_Name, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            లోపల_Name+=8;
            c=*లోపల_Name;
            ఒకవేళ(c==0) {
                p_వాదనలు->options=(pPieces->options&=~UCNV_OPTION_సంస్కరణ);
                తిరిగి;
            } లేకపోతే ఒకవేళ (static_cast<uint8_t>(c - '0') < 10) {
                p_వాదనలు->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_సంస్కరణ) | static_cast<uint32_t>(c - '0');
                ++లోపల_Name;
            }
        } లేకపోతే ఒకవేళ(uprv_strncmp(లోపల_Name, "swaplfnl", 8)==0) {
            లోపల_Name+=8;
            p_వాదనలు->options=(pPieces->options|=UCNV_OPTION_మార్పిడి_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } లేకపోతే {
            /* ignore any other options until we define some */
            అయితే(((c = *లోపల_Name++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            ఒకవేళ(c==0) {
                తిరిగి;
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
U_CFUNC UConverter_Shared_దత్తాంశం *
ucnv_ఎక్కించు_Shared_దత్తాంశం(స్థిరరాశి అక్షరం *converterName,
                    UConverterNamePieces *pPieces,
                    UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
                    UError_సంకేతం * err) {
    UConverterNamePieces దొంతర_Pieces;
    UConverter_ఎక్కించు_వాదనలు దొంతర_వాదనలు;
    UConverter_Shared_దత్తాంశం *my_Shared_Converter_దత్తాంశం = శూన్యసూచిక;
    UError_సంకేతం అంతర్గత_Error_సంకేతం = U_ZERO_ERROR;
    UBool mayContainOption = నిజం;
    UBool checkForAlgorithmic = నిజం;

    ఒకవేళ (U_FAILURE (*err)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(pPieces == శూన్యసూచిక) {
        ఒకవేళ(p_వాదనలు != శూన్యసూచిక) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_అంతర్గత_కార్యక్రమం_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        pPieces = &దొంతర_Pieces;
    }
    ఒకవేళ(p_వాదనలు == శూన్యసూచిక) {
        uprv_memset(&దొంతర_వాదనలు, 0, పరిమాణం(దొంతర_వాదనలు));
        దొంతర_వాదనలు.size = (int32_t)పరిమాణం(దొంతర_వాదనలు);
        p_వాదనలు = &దొంతర_వాదనలు;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    p_వాదనలు->name = converterName;
    p_వాదనలు->locale = pPieces->locale;
    p_వాదనలు->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    ఒకవేళ (converterName == శూన్యసూచిక) {
#ఒకవేళ U_CHARSET_IS_UTF8
        p_వాదనలు->name = "UTF-8";
        తిరిగి (UConverter_Shared_దత్తాంశం *)converter_దత్తాంశం[UCNV_UTF8];
#లేకపోతే
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        p_వాదనలు->name = ucnv_getDefaultName();
        ఒకవేళ (p_వాదనలు->name == శూన్యసూచిక) {
            *err = U_MISSING_RESOURCE_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        my_Shared_Converter_దత్తాంశం = (UConverter_Shared_దత్తాంశం *)g_Default_Algorithmic_Shared_దత్తాంశం;
        checkForAlgorithmic = అబద్ధం;
        mayContainOption = g_Default_Converter_కలిగియుంది_Option;
        /* the default converter name is already canonical */
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        p_వాదనలు->name = "UTF-8";
        తిరిగి (UConverter_Shared_దత్తాంశం *)converter_దత్తాంశం[UCNV_UTF8];
    }
    లేకపోతే {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, p_వాదనలు, err);
        ఒకవేళ (U_FAILURE(*err)) {
            /* Very bad name used. */
            తిరిగి శూన్యసూచిక;
        }

        /* get the canonical converter name */
        p_వాదనలు->name = ucnv_io_getConverterName(p_వాదనలు->name, &mayContainOption, &అంతర్గత_Error_సంకేతం);
        ఒకవేళ (U_FAILURE(అంతర్గత_Error_సంకేతం) || p_వాదనలు->name == శూన్యసూచిక) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            p_వాదనలు->name = pPieces->cnvName;
        } లేకపోతే ఒకవేళ (అంతర్గత_Error_సంకేతం == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    ఒకవేళ(mayContainOption && p_వాదనలు->name != pPieces->cnvName) {
        parseConverterOptions(p_వాదనలు->name, pPieces, p_వాదనలు, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    ఒకవేళ (checkForAlgorithmic) {
        my_Shared_Converter_దత్తాంశం = (UConverter_Shared_దత్తాంశం *)getAlgorithmicTypeFromName(p_వాదనలు->name);
    }
    ఒకవేళ (my_Shared_Converter_దత్తాంశం == శూన్యసూచిక)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        p_వాదనలు->nestedLoads=1;
        p_వాదనలు->pkg=శూన్యసూచిక;

        umtx_తాళం(&cnvCacheMutex);
        my_Shared_Converter_దత్తాంశం = ucnv_ఎక్కించు(p_వాదనలు, err);
        umtx_unlock(&cnvCacheMutex);
        ఒకవేళ (U_FAILURE (*err) || (my_Shared_Converter_దత్తాంశం == శూన్యసూచిక))
        {
            తిరిగి శూన్యసూచిక;
        }
    }

    తిరిగి my_Shared_Converter_దత్తాంశం;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     స్థిరరాశి అక్షరం* converterName,
                     UError_సంకేతం* err)
{
    UConverterNamePieces దొంతర_Pieces;
    UConverter_ఎక్కించు_వాదనలు దొంతర_వాదనలు=UCNV_ఎక్కించు_వాదనలు_INITIALIZER;
    UConverter_Shared_దత్తాంశం *my_Shared_Converter_దత్తాంశం;

    UTRACE_నమోదు_OC(UTRACE_UCNV_OPEN);

    ఒకవేళ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_దత్తాంశం = ucnv_ఎక్కించు_Shared_దత్తాంశం(converterName, &దొంతర_Pieces, &దొంతర_వాదనలు, err);

        myUConverter = ucnv_create_Converter_From_Shared_దత్తాంశం(
            myUConverter, my_Shared_Converter_దత్తాంశం,
            &దొంతర_వాదనలు,
            err);

        ఒకవేళ(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_స్థితి(myUConverter, *err);
            తిరిగి myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_స్థితి(*err);
    తిరిగి శూన్యసూచిక;
}

U_CFUNC UBool
ucnv_canCreateConverter(స్థిరరాశి అక్షరం *converterName, UError_సంకేతం *err) {
    UConverter myUConverter;
    UConverterNamePieces దొంతర_Pieces;
    UConverter_ఎక్కించు_వాదనలు దొంతర_వాదనలు=UCNV_ఎక్కించు_వాదనలు_INITIALIZER;
    UConverter_Shared_దత్తాంశం *my_Shared_Converter_దత్తాంశం;

    UTRACE_నమోదు_OC(UTRACE_UCNV_OPEN);

    ఒకవేళ(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        దొంతర_వాదనలు.onlyTestIsLoadable=నిజం;
        my_Shared_Converter_దత్తాంశం = ucnv_ఎక్కించు_Shared_దత్తాంశం(converterName, &దొంతర_Pieces, &దొంతర_వాదనలు, err);
        ucnv_create_Converter_From_Shared_దత్తాంశం(
            &myUConverter, my_Shared_Converter_దత్తాంశం,
            &దొంతర_వాదనలు,
            err);
        ucnv_unload_Shared_దత్తాంశం_If_Ready(my_Shared_Converter_దత్తాంశం);
    }

    UTRACE_EXIT_స్థితి(*err);
    తిరిగి U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                స్థిరరాశి అక్షరం* locale,
                                uint32_t options,
                                UError_సంకేతం* err) {
    UConverter *cnv;
    స్థిరరాశి UConverter_Shared_దత్తాంశం *shared_దత్తాంశం;
    UConverter_ఎక్కించు_వాదనలు దొంతర_వాదనలు=UCNV_ఎక్కించు_వాదనలు_INITIALIZER;

    UTRACE_నమోదు_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    ఒకవేళ(type<0 || UCNV_సంఖ్య_యొక్క_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_వాదం_ERROR;
        UTRACE_EXIT_స్థితి(U_ILLEGAL_వాదం_ERROR);
        తిరిగి శూన్యసూచిక;
    }

    shared_దత్తాంశం = converter_దత్తాంశం[type];
    ఒకవేళ(shared_దత్తాంశం == శూన్యసూచిక || shared_దత్తాంశం->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_వాదం_ERROR;
        UTRACE_EXIT_స్థితి(U_ILLEGAL_వాదం_ERROR);
        తిరిగి శూన్యసూచిక;
    }

    దొంతర_వాదనలు.name = "";
    దొంతర_వాదనలు.options = options;
    దొంతర_వాదనలు.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_దత్తాంశం(
            myUConverter, const_cast<UConverter_Shared_దత్తాంశం*>(shared_దత్తాంశం),
            &దొంతర_వాదనలు, err);

    UTRACE_EXIT_PTR_స్థితి(cnv, *err);
    తిరిగి cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(స్థిరరాశి అక్షరం *packageName, స్థిరరాశి అక్షరం *converterName, UError_సంకేతం * err)
{
    UConverter *myUConverter;
    UConverter_Shared_దత్తాంశం *my_Shared_Converter_దత్తాంశం;
    UConverterNamePieces దొంతర_Pieces;
    UConverter_ఎక్కించు_వాదనలు దొంతర_వాదనలు=UCNV_ఎక్కించు_వాదనలు_INITIALIZER;

    UTRACE_నమోదు_OC(UTRACE_UCNV_OPEN_PACKAGE);

    ఒకవేళ(U_FAILURE(*err)) {
        UTRACE_EXIT_స్థితి(*err);
        తిరిగి శూన్యసూచిక;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    దొంతర_Pieces.cnvName[0] = 0;
    దొంతర_Pieces.locale[0] = 0;
    దొంతర_Pieces.options = 0;
    parseConverterOptions(converterName, &దొంతర_Pieces, &దొంతర_వాదనలు, err);
    ఒకవేళ (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_స్థితి(*err);
        తిరిగి శూన్యసూచిక;
    }
    దొంతర_వాదనలు.nestedLoads=1;
    దొంతర_వాదనలు.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_దత్తాంశం = createConverterFromFile(&దొంతర_వాదనలు, err);

    ఒకవేళ (U_FAILURE(*err)) {
        UTRACE_EXIT_స్థితి(*err);
        తిరిగి శూన్యసూచిక;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_దత్తాంశం(శూన్యసూచిక, my_Shared_Converter_దత్తాంశం, &దొంతర_వాదనలు, err);

    ఒకవేళ (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_స్థితి(*err);
        తిరిగి శూన్యసూచిక;
    }

    UTRACE_EXIT_PTR_స్థితి(myUConverter, *err);
    తిరిగి myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_దత్తాంశం(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_దత్తాంశం* my_Shared_Converter_దత్తాంశం,
                                   UConverter_ఎక్కించు_వాదనలు* p_వాదనలు,
                                   UError_సంకేతం* err)
{
    UBool isCopyLocal;

    ఒకవేళ(U_FAILURE(*err)) {
        ucnv_unload_Shared_దత్తాంశం_If_Ready(my_Shared_Converter_దత్తాంశం);
        తిరిగి myUConverter;
    }
    ఒకవేళ(myUConverter == శూన్యసూచిక)
    {
        myUConverter = (UConverter *) uprv_malloc (పరిమాణం (UConverter));
        ఒకవేళ(myUConverter == శూన్యసూచిక)
        {
            *err = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            ucnv_unload_Shared_దత్తాంశం_If_Ready(my_Shared_Converter_దత్తాంశం);
            తిరిగి శూన్యసూచిక;
        }
        isCopyLocal = అబద్ధం;
    } లేకపోతే {
        isCopyLocal = నిజం;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, పరిమాణం(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_దత్తాంశం = my_Shared_Converter_దత్తాంశం;
    myUConverter->options = p_వాదనలు->options;
    ఒకవేళ(!p_వాదనలు->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_గమ్యం_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->గమ్యం_Unicode_స్థితి = my_Shared_Converter_దత్తాంశం->గమ్యం_Unicode_స్థితి;
        myUConverter->max_బైట్లు_చొప్పున_UChar = my_Shared_Converter_దత్తాంశం->static_దత్తాంశం->max_బైట్లు_చొప్పున_Char;
        myUConverter->subChar1 = my_Shared_Converter_దత్తాంశం->static_దత్తాంశం->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_దత్తాంశం->static_దత్తాంశం->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_దత్తాంశం->static_దత్తాంశం->subChar, myUConverter->subCharLen);
        myUConverter->గమ్యం_UCallback_Reason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    ఒకవేళ(my_Shared_Converter_దత్తాంశం->impl->open != శూన్యసూచిక) {
        my_Shared_Converter_దత్తాంశం->impl->open(myUConverter, p_వాదనలు, err);
        ఒకవేళ(U_FAILURE(*err) && !p_వాదనలు->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            తిరిగి శూన్యసూచిక;
        }
    }

    తిరిగి myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_దత్తాంశం *my_Shared_దత్తాంశం = శూన్యసూచిక;
    int32_t pos;
    int32_t పట్టిక_Deleted_అంకె = 0;
    స్థిరరాశి UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, మిగిలిన;

    UTRACE_నమోదు_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    ఒకవేళ (SHARED_దత్తాంశం_HASHTABLE == శూన్యసూచిక) {
        UTRACE_EXIT_విలువ((int32_t)0);
        తిరిగి 0;
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
    umtx_తాళం(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    చేయి {
        మిగిలిన = 0;
        pos = UHASH_మొదటి;
        అయితే ((e = uhash_తదుపరి_Element (SHARED_దత్తాంశం_HASHTABLE, &pos)) != శూన్యసూచిక)
        {
            my_Shared_దత్తాంశం = (UConverter_Shared_దత్తాంశం *) e->value.సూచిక;
            /*deletes only if reference counter == 0 */
            ఒకవేళ (my_Shared_దత్తాంశం->referenceCounter == 0)
            {
                పట్టిక_Deleted_అంకె++;

                UCNV_DEBUG_LOG("del",my_Shared_దత్తాంశం->static_దత్తాంశం->name,my_Shared_దత్తాంశం);

                uhash_removeElement(SHARED_దత్తాంశం_HASHTABLE, e);
                my_Shared_దత్తాంశం->shared_దత్తాంశం_Cached = అబద్ధం;
                ucnv_delete_Shared_Converter_దత్తాంశం (my_Shared_దత్తాంశం);
            } లేకపోతే {
                ++మిగిలిన;
            }
        }
    } అయితే(++i == 1 && మిగిలిన > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", మిగిలిన);

    UTRACE_EXIT_విలువ(పట్టిక_Deleted_అంకె);
    తిరిగి పట్టిక_Deleted_అంకె;
}

/* available converters list --------------------------------------------------- */

స్థిర శూన్య U_CALLCONV init_అందుబాటు_Converters_List(UError_సంకేతం &err_సంకేతం) {
    U_ASSERT(g_అందుబాటు_Converter_లెక్క == 0);
    U_ASSERT(g_అందుబాటు_Converters == శూన్యసూచిక);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_open_All_పేర్లు(&err_సంకేతం);
    int32_t all_Converter_లెక్క = uenum_లెక్క(allConvEnum, &err_సంకేతం);
    ఒకవేళ (U_FAILURE(err_సంకేతం)) {
        తిరిగి;
    }

    /* We can't have more than "*converterTable" converters to open */
    g_అందుబాటు_Converters = static_cast<స్థిరరాశి అక్షరం**>(uprv_malloc(all_Converter_లెక్క * పరిమాణం(అక్షరం*)));
    ఒకవేళ (!g_అందుబాటు_Converters) {
        err_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UError_సంకేతం local_స్థితి = U_ZERO_ERROR;
    UConverter తాత్కాలిక_Converter;
    ucnv_close(ucnv_createConverter(&తాత్కాలిక_Converter, శూన్యసూచిక, &local_స్థితి));

    g_అందుబాటు_Converter_లెక్క = 0;

    కొరకు (int32_t idx = 0; idx < all_Converter_లెక్క; idx++) {
        local_స్థితి = U_ZERO_ERROR;
        స్థిరరాశి అక్షరం *converterName = uenum_తదుపరి(allConvEnum, శూన్యసూచిక, &local_స్థితి);
        ఒకవేళ (ucnv_canCreateConverter(converterName, &local_స్థితి)) {
            g_అందుబాటు_Converters[g_అందుబాటు_Converter_లెక్క++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


స్థిర UBool have_అందుబాటు_Converter_List(UError_సంకేతం *p_Error_సంకేతం) {
    umtx_initOnce(g_అందుబాటు_Converters_Init_Once, &init_అందుబాటు_Converters_List, *p_Error_సంకేతం);
    తిరిగి U_SUCCESS(*p_Error_సంకేతం);
}

U_CFUNC uint16_t
ucnv_bld_లెక్క_అందుబాటు_Converters(UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (have_అందుబాటు_Converter_List(p_Error_సంకేతం)) {
        తిరిగి g_అందుబాటు_Converter_లెక్క;
    }
    తిరిగి 0;
}

U_CFUNC స్థిరరాశి అక్షరం *
ucnv_bld_get_అందుబాటు_Converter(uint16_t n, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (have_అందుబాటు_Converter_List(p_Error_సంకేతం)) {
        ఒకవేళ (n < g_అందుబాటు_Converter_లెక్క) {
            తిరిగి g_అందుబాటు_Converters[n];
        }
        *p_Error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
    }
    తిరిగి శూన్యసూచిక;
}

/* default converter name --------------------------------------------------- */

#ఒకవేళ !U_CHARSET_IS_UTF8
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
స్థిర అంతఃవరుస శూన్య
అంతర్గత_సమితి_Name(స్థిరరాశి అక్షరం *name, UError_సంకేతం *స్థితి_2) {
    UConverterNamePieces దొంతర_Pieces;
    UConverter_ఎక్కించు_వాదనలు దొంతర_వాదనలు=UCNV_ఎక్కించు_వాదనలు_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool కలిగియుంది_Option = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != శూన్యసూచిక);
    స్థిరరాశి UConverter_Shared_దత్తాంశం *algorithmic_Shared_దత్తాంశం;

    దొంతర_వాదనలు.name = name;
    ఒకవేళ(కలిగియుంది_Option) {
        దొంతర_Pieces.cnvName[0] = 0;
        దొంతర_Pieces.locale[0] = 0;
        దొంతర_Pieces.options = 0;
        parseConverterOptions(name, &దొంతర_Pieces, &దొంతర_వాదనలు, స్థితి_2);
        ఒకవేళ(U_FAILURE(*స్థితి_2)) {
            తిరిగి;
        }
    }
    algorithmic_Shared_దత్తాంశం = getAlgorithmicTypeFromName(దొంతర_వాదనలు.name);

    umtx_తాళం(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_దత్తాంశం = algorithmic_Shared_దత్తాంశం;
    g_Default_Converter_కలిగియుంది_Option = కలిగియుంది_Option;
    uprv_memcpy(g_Default_Converter_Name_బఫరు, name, length);
    g_Default_Converter_Name_బఫరు[length]=0;

    /* gDefaultConverterName MUST be the last global var set by this function.  */
    /*    It is the variable checked in ucnv_getDefaultName() to see if initialization is required. */
    //    But there is nothing here preventing that from being reordered, either by the compiler
    //             or hardware. I'm adding the mutex to ucnv_getDefaultName for now. UMTX_CHECK is not enough.
    //             -- Andy
    gDefaultConverterName = g_Default_Converter_Name_బఫరు;

    ucnv_enableCleanup();

    umtx_unlock(&cnvCacheMutex);
}
#అయితే_ముగింపు

/*
 * In order to be really thread-safe, the get function would have to take
 * a buffer parameter and copy the current string inside a mutex block.
 * This implementation only tries to be really thread-safe while
 * setting the name.
 * It assumes that setting a pointer is atomic.
 */

U_CAPI స్థిరరాశి అక్షరం*  U_EXPORT2
ucnv_getDefaultName() {
#ఒకవేళ U_CHARSET_IS_UTF8
    తిరిగి "UTF-8";
#లేకపోతే
    /* local variable to be thread-safe */
    స్థిరరాశి అక్షరం *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex తాళం(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    ఒకవేళ(name==శూన్యసూచిక) {
        UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
        UConverter *cnv = శూన్యసూచిక;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        ఒకవేళ(name != శూన్యసూచిక) {
            cnv = ucnv_open(name, &error_సంకేతం);
            ఒకవేళ(U_SUCCESS(error_సంకేతం) && cnv != శూన్యసూచిక) {
                name = ucnv_getName(cnv, &error_సంకేతం);
            }
        }

        ఒకవేళ(name == శూన్యసూచిక || name[0] == 0
            || U_FAILURE(error_సంకేతం) || cnv == శూన్యసూచిక
            || uprv_strlen(name)>=పరిమాణం(g_Default_Converter_Name_బఫరు))
        {
            /* Panic time, let's use a fallback. */
#ఒకవేళ (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#లేదావేళ U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_మార్పిడి_LFNL_OPTION_STRING;
#లేకపోతే
            name = "ibm-37_P100-1995";
#అయితే_ముగింపు
        }

        అంతర్గత_సమితి_Name(name, &error_సంకేతం);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    తిరిగి name;
#అయితే_ముగింపు
}

#ఒకవేళ U_CHARSET_IS_UTF8
U_CAPI శూన్య U_EXPORT2 ucnv_సమితి_Default_Name(స్థిరరాశి అక్షరం *) {}
#లేకపోతే
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI శూన్య U_EXPORT2
ucnv_సమితి_Default_Name(స్థిరరాశి అక్షరం *converterName) {
    ఒకవేళ(converterName==శూన్యసూచిక) {
        /* reset to the default codepage */
        gDefaultConverterName=శూన్యసూచిక;
    } లేకపోతే {
        UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
        UConverter *cnv = శూన్యసూచిక;
        స్థిరరాశి అక్షరం *name = శూన్యసూచిక;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &error_సంకేతం);
        ఒకవేళ(U_SUCCESS(error_సంకేతం) && cnv != శూన్యసూచిక) {
            name = ucnv_getName(cnv, &error_సంకేతం);
        }

        ఒకవేళ(U_SUCCESS(error_సంకేతం) && name!=శూన్యసూచిక) {
            అంతర్గత_సమితి_Name(name, &error_సంకేతం);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#అయితే_ముగింపు

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#ఒకవేళ !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
          స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
          UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UDataInfo *pInfo;
    int32_t శీర్షిక_కొలత;

    స్థిరరాశి uint8_t *లోపల_బైట్లు;
    uint8_t *బహిర్గతం_బైట్లు;

    uint32_t స్థానభ్రంశం, count, static_దత్తాంశం_కొలత;
    int32_t size;

    స్థిరరాశి UConverter_Static_దత్తాంశం *లోపల_Static_దత్తాంశం;
    UConverter_Static_దత్తాంశం *బహిర్గతం_Static_దత్తాంశం;

    స్థిరరాశి _MBCSHeader *లోపల_MBCSHeader;
    _MBCSHeader *బహిర్గతం_MBCSHeader;
    _MBCSHeader mbcs_శీర్షిక;
    uint32_t mbcs_శీర్షిక_నిడివి;
    UBool noFromU=అబద్ధం;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_సూచీ_నిడివి;

    స్థిరరాశి int32_t *లోపల_Ext_Indexes;
    int32_t ext_స్థానభ్రంశం;

    /* udata_swapDataHeader checks the arguments */
    శీర్షిక_కొలత=udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* check data format and format version */
    pInfo=(స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
    ఒకవేళ(!(
        pInfo->దత్తాంశం_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->దత్తాంశం_Format[1]==0x6e &&
        pInfo->దత్తాంశం_Format[2]==0x76 &&
        pInfo->దత్తాంశం_Format[3]==0x74 &&
        pInfo->format_సంస్కరణ[0]==6 &&
        pInfo->format_సంస్కరణ[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1],
                         pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3],
                         pInfo->format_సంస్కరణ[0], pInfo->format_సంస్కరణ[1]);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    లోపల_బైట్లు=(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం+శీర్షిక_కొలత;
    బహిర్గతం_బైట్లు=(బహిర్గతం_దత్తాంశం == శూన్యసూచిక) ? శూన్యసూచిక : (uint8_t *)బహిర్గతం_దత్తాంశం+శీర్షిక_కొలత;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    లోపల_Static_దత్తాంశం=(స్థిరరాశి UConverter_Static_దత్తాంశం *)లోపల_బైట్లు;
    బహిర్గతం_Static_దత్తాంశం=(UConverter_Static_దత్తాంశం *)బహిర్గతం_బైట్లు;

    ఒకవేళ(length<0) {
        static_దత్తాంశం_కొలత=ds->readUInt32(లోపల_Static_దత్తాంశం->struct_కొలత);
    } లేకపోతే {
        length-=శీర్షిక_కొలత;
        ఒకవేళ( length<(int32_t)పరిమాణం(UConverter_Static_దత్తాంశం) ||
            (uint32_t)length<(static_దత్తాంశం_కొలత=ds->readUInt32(లోపల_Static_దత్తాంశం->struct_కొలత))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
    }

    ఒకవేళ(length>=0) {
        /* swap the static data */
        ఒకవేళ(లోపల_Static_దత్తాంశం!=బహిర్గతం_Static_దత్తాంశం) {
            uprv_memcpy(బహిర్గతం_Static_దత్తాంశం, లోపల_Static_దత్తాంశం, static_దత్తాంశం_కొలత);
        }

        ds->మార్పిడి_Array32(ds, &లోపల_Static_దత్తాంశం->struct_కొలత, 4,
                           &బహిర్గతం_Static_దత్తాంశం->struct_కొలత, p_Error_సంకేతం);
        ds->మార్పిడి_Array32(ds, &లోపల_Static_దత్తాంశం->codepage, 4,
                           &బహిర్గతం_Static_దత్తాంశం->codepage, p_Error_సంకేతం);

        ds->మార్పిడి_Inv_Chars(ds, లోపల_Static_దత్తాంశం->name, (int32_t)uprv_strlen(లోపల_Static_దత్తాంశం->name),
                            బహిర్గతం_Static_దత్తాంశం->name, p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            తిరిగి 0;
        }
    }

    లోపల_బైట్లు+=static_దత్తాంశం_కొలత;
    ఒకవేళ (బహిర్గతం_బైట్లు != శూన్యసూచిక) బహిర్గతం_బైట్లు+=static_దత్తాంశం_కొలత;
    ఒకవేళ(length>=0) {
        length-=(int32_t)static_దత్తాంశం_కొలత;
    }

    /* check for supported conversionType values */
    ఒకవేళ(లోపల_Static_దత్తాంశం->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        లోపల_MBCSHeader=(స్థిరరాశి _MBCSHeader *)లోపల_బైట్లు;
        బహిర్గతం_MBCSHeader=(_MBCSHeader *)బహిర్గతం_బైట్లు;

        ఒకవేళ(0<=length && length<(int32_t)పరిమాణం(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
        ఒకవేళ(లోపల_MBCSHeader->సంస్కరణ[0]==4 && లోపల_MBCSHeader->సంస్కరణ[1]>=1) {
            mbcs_శీర్షిక_నిడివి=MBCS_శీర్షిక_V4_నిడివి;
        } లేకపోతే ఒకవేళ(లోపల_MBCSHeader->సంస్కరణ[0]==5 && లోపల_MBCSHeader->సంస్కరణ[1]>=3 &&
                  ((mbcs_శీర్షిక.options=ds->readUInt32(లోపల_MBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_ముసుగు)==0
        ) {
            mbcs_శీర్షిక_నిడివి=mbcs_శీర్షిక.options&MBCS_OPT_నిడివి_ముసుగు;
            noFromU = (mbcs_శీర్షిక.options & MBCS_OPT_NO_FROM_U) != 0;
        } లేకపోతే {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             లోపల_MBCSHeader->సంస్కరణ[0], లోపల_MBCSHeader->సంస్కరణ[1]);
            *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
            తిరిగి 0;
        }

        uprv_memcpy(mbcs_శీర్షిక.సంస్కరణ, లోపల_MBCSHeader->సంస్కరణ, 4);
        mbcs_శీర్షిక.లెక్క_స్థితులు=         ds->readUInt32(లోపల_MBCSHeader->లెక్క_స్థితులు);
        mbcs_శీర్షిక.లెక్క_గమ్యం_UFallbacks=   ds->readUInt32(లోపల_MBCSHeader->లెక్క_గమ్యం_UFallbacks);
        mbcs_శీర్షిక.స్థానభ్రంశం_గమ్యం_UCode_Units=  ds->readUInt32(లోపల_MBCSHeader->స్థానభ్రంశం_గమ్యం_UCode_Units);
        mbcs_శీర్షిక.స్థానభ్రంశం_From_UTable=    ds->readUInt32(లోపల_MBCSHeader->స్థానభ్రంశం_From_UTable);
        mbcs_శీర్షిక.స్థానభ్రంశం_From_UBytes=    ds->readUInt32(లోపల_MBCSHeader->స్థానభ్రంశం_From_UBytes);
        mbcs_శీర్షిక.flags=               ds->readUInt32(లోపల_MBCSHeader->flags);
        mbcs_శీర్షిక.from_UBytes_నిడివి=    ds->readUInt32(లోపల_MBCSHeader->from_UBytes_నిడివి);
        /* mbcsHeader.options have been read above */

        ext_స్థానభ్రంశం=(int32_t)(mbcs_శీర్షిక.flags>>8);
        outputType=(uint8_t)mbcs_శీర్షిక.flags;
        ఒకవేళ(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
            తిరిగి 0;
        }

        /* make sure that the output type is known */
        ఎంపిక(outputType) {
        స్థితి MBCS_OUTPUT_1:
        స్థితి MBCS_OUTPUT_2:
        స్థితి MBCS_OUTPUT_3:
        స్థితి MBCS_OUTPUT_4:
        స్థితి MBCS_OUTPUT_3_EUC:
        స్థితి MBCS_OUTPUT_4_EUC:
        స్థితి MBCS_OUTPUT_2_SISO:
        స్థితి MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            ఆపు;
        అప్రమేయ:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
            తిరిగి 0;
        }

        /* calculate the length of the MBCS data */

        /*
         * utf8Friendly MBCS files (mbcsHeader.version 4.3)
         * contain an additional mbcsIndex table:
         *   uint16_t[(maxFastUChar+1)>>6];
         * where maxFastUChar=((mbcsHeader.version[2]<<8)|0xff).
         */
        maxFastUChar=0;
        mbcs_సూచీ_నిడివి=0;
        ఒకవేళ( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcs_శీర్షిక.సంస్కరణ[1]>=3 && (maxFastUChar=mbcs_శీర్షిక.సంస్కరణ[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_సూచీ_నిడివి=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        ఒకవేళ(ext_స్థానభ్రంశం==0) {
            size=(int32_t)(mbcs_శీర్షిక.స్థానభ్రంశం_From_UBytes+mbcs_సూచీ_నిడివి);
            ఒకవేళ(!noFromU) {
                size+=(int32_t)mbcs_శీర్షిక.from_UBytes_నిడివి;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            లోపల_Ext_Indexes=శూన్యసూచిక;
        } లేకపోతే {
            /* there is extension data after the base data, see ucnv_ext.h */
            ఒకవేళ(length>=0 && length<(ext_స్థానభ్రంశం+UCNV_EXT_INDEXES_MIN_నిడివి*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                తిరిగి 0;
            }

            లోపల_Ext_Indexes=(స్థిరరాశి int32_t *)(లోపల_బైట్లు+ext_స్థానభ్రంశం);
            size=ext_స్థానభ్రంశం+udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_కొలత]);
        }

        ఒకవేళ(length>=0) {
            ఒకవేళ(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                తిరిగి 0;
            }

            /* copy the data for inaccessible bytes */
            ఒకవేళ(లోపల_బైట్లు!=బహిర్గతం_బైట్లు) {
                uprv_memcpy(బహిర్గతం_బైట్లు, లోపల_బైట్లు, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_శీర్షిక_నిడివి*4;
            ds->మార్పిడి_Array32(ds, &లోపల_MBCSHeader->లెక్క_స్థితులు, count-4,
                               &బహిర్గతం_MBCSHeader->లెక్క_స్థితులు, p_Error_సంకేతం);

            ఒకవేళ(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                స్థిరరాశి అక్షరం *లోపల_పునాది_Name=(స్థిరరాశి అక్షరం *)లోపల_బైట్లు+count;
                అక్షరం *బహిర్గతం_పునాది_Name=(అక్షరం *)బహిర్గతం_బైట్లు+count;
                ds->మార్పిడి_Inv_Chars(ds, లోపల_పునాది_Name, (int32_t)uprv_strlen(లోపల_పునాది_Name),
                                    బహిర్గతం_పునాది_Name, p_Error_సంకేతం);
            } లేకపోతే {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                స్థానభ్రంశం=count;
                count=mbcs_శీర్షిక.లెక్క_స్థితులు*1024;
                ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+స్థానభ్రంశం, (int32_t)count,
                                   బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* swap the toUFallbacks[] */
                స్థానభ్రంశం+=count;
                count=mbcs_శీర్షిక.లెక్క_గమ్యం_UFallbacks*8;
                ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+స్థానభ్రంశం, (int32_t)count,
                                   బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* swap the unicodeCodeUnits[] */
                స్థానభ్రంశం=mbcs_శీర్షిక.స్థానభ్రంశం_గమ్యం_UCode_Units;
                count=mbcs_శీర్షిక.స్థానభ్రంశం_From_UTable-స్థానభ్రంశం;
                ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, (int32_t)count,
                                   బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* offset to the stage 1 table, independent of the outputType */
                స్థానభ్రంశం=mbcs_శీర్షిక.స్థానభ్రంశం_From_UTable;

                ఒకవేళ(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcs_శీర్షిక.స్థానభ్రంశం_From_UBytes-స్థానభ్రంశం)+mbcs_శీర్షిక.from_UBytes_నిడివి;
                    ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, (int32_t)count,
                                       బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);
                } లేకపోతే {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    ఒకవేళ(లోపల_Static_దత్తాంశం->unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } లేకపోతే {
                        count=0x40*2; /* only BMP */
                    }
                    ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, (int32_t)count,
                                       బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                    /* stage 2 table: uint32_t[] */
                    స్థానభ్రంశం+=count;
                    count=mbcs_శీర్షిక.స్థానభ్రంశం_From_UBytes-స్థానభ్రంశం;
                    ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+స్థానభ్రంశం, (int32_t)count,
                                       బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    స్థానభ్రంశం=mbcs_శీర్షిక.స్థానభ్రంశం_From_UBytes;
                    count= noFromU ? 0 : mbcs_శీర్షిక.from_UBytes_నిడివి;
                    ఎంపిక(outputType) {
                    స్థితి MBCS_OUTPUT_2:
                    స్థితి MBCS_OUTPUT_3_EUC:
                    స్థితి MBCS_OUTPUT_2_SISO:
                        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, (int32_t)count,
                                           బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);
                        ఆపు;
                    స్థితి MBCS_OUTPUT_4:
                        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+స్థానభ్రంశం, (int32_t)count,
                                           బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);
                        ఆపు;
                    అప్రమేయ:
                        /* just uint8_t[], nothing to swap */
                        ఆపు;
                    }

                    ఒకవేళ(mbcs_సూచీ_నిడివి!=0) {
                        స్థానభ్రంశం+=count;
                        count=mbcs_సూచీ_నిడివి;
                        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, (int32_t)count,
                                           బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);
                    }
                }
            }

            ఒకవేళ(ext_స్థానభ్రంశం!=0) {
                /* swap the extension data */
                లోపల_బైట్లు+=ext_స్థానభ్రంశం;
                బహిర్గతం_బైట్లు+=ext_స్థానభ్రంశం;

                /* swap toUTable[] */
                స్థానభ్రంశం=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_గమ్యం_U_సూచీ]);
                length=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_గమ్యం_U_నిడివి]);
                ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+స్థానభ్రంశం, length*4, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* swap toUUChars[] */
                స్థానభ్రంశం=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_గమ్యం_U_UCHARS_సూచీ]);
                length=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_గమ్యం_U_UCHARS_నిడివి]);
                ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, length*2, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* swap fromUTableUChars[] */
                స్థానభ్రంశం=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_FROM_U_UCHARS_సూచీ]);
                length=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_FROM_U_నిడివి]);
                ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, length*2, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* swap fromUTableValues[] */
                స్థానభ్రంశం=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_FROM_U_విలువలు_సూచీ]);
                /* same length as for fromUTableUChars[] */
                ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+స్థానభ్రంశం, length*4, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                స్థానభ్రంశం=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_సూచీ]);
                length=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_నిడివి]);
                ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, length*2, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* swap fromUStage3[] */
                స్థానభ్రంశం=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_సూచీ]);
                length=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_నిడివి]);
                ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, length*2, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* swap fromUStage3b[] */
                స్థానభ్రంశం=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_సూచీ]);
                length=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_నిడివి]);
                ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+స్థానభ్రంశం, length*4, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);

                /* swap indexes[] */
                length=udata_readInt32(ds, లోపల_Ext_Indexes[UCNV_EXT_INDEXES_నిడివి]);
                ds->మార్పిడి_Array32(ds, లోపల_బైట్లు, length*4, బహిర్గతం_బైట్లు, p_Error_సంకేతం);
            }
        }
    } లేకపోతే {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         లోపల_Static_దత్తాంశం->conversionType);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    తిరిగి శీర్షిక_కొలత+(int32_t)static_దత్తాంశం_కొలత+size;
}

#అయితే_ముగింపు /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#అయితే_ముగింపు
