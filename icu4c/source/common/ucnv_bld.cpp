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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "utracimp.h"
#अन्तर्भाव "ucnv_io.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnvmbcs.h"
#अन्तर्भाव "ucnv_ext.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnv_imp.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "ustr_cnv.h"


#यदि 0
#अन्तर्भाव <stdio.h>
बाह्य शून्य UCNV_DEBUG_LOG(वर्ण *what, वर्ण *who, शून्य *p, पूर्णाङ्क l);
#परिभाषय UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#अन्यथा
# define UCNV_DEBUG_LOG(x,y,z)
#यदि_अन्तः

स्थिर ध्रुव UConverter_Shared_दत्त * ध्रुव
converter_दत्त[UCNV_सङ्ख्या_सम्बन्धि_SUPPORTED_CONVERTER_TYPES]={
    शून्यसूचकः, शून्यसूचकः,

#यदि UCONFIG_NO_LEGACY_CONVERSION
    शून्यसूचकः,
#अन्यथा
    &_MBCSData,
#यदि_अन्तः

    &Latin1_दत्त,
    &UTF8_दत्त, &_UTF16BEData, &_UTF16LEData,
#यदि UCONFIG_ONLY_HTML_CONVERSION
    शून्यसूचकः, शून्यसूचकः,
#अन्यथा
    &_UTF32BEData, &_UTF32LEData,
#यदि_अन्तः
    शून्यसूचकः,

#यदि UCONFIG_NO_LEGACY_CONVERSION
    शून्यसूचकः,
#अन्यथा
    &ISO2022_दत्त,
#यदि_अन्तः

#यदि UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः,
    शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः,
    शून्यसूचकः,
#अन्यथा
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#यदि_अन्तः

#यदि UCONFIG_ONLY_HTML_CONVERSION
    शून्यसूचकः,
#अन्यथा
    &_SCSUData,
#यदि_अन्तः


#यदि UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    शून्यसूचकः,
#अन्यथा
    &_ISCIIData,
#यदि_अन्तः

    &_ASCIIData,
#यदि UCONFIG_ONLY_HTML_CONVERSION
    शून्यसूचकः, शून्यसूचकः, &UTF16_दत्त, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः,
#अन्यथा
    &UTF7_दत्त, &Bocu1_दत्त, &UTF16_दत्त, &UTF32_दत्त, &CESU8_दत्त, &_IMAPData,
#यदि_अन्तः

#यदि UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    शून्यसूचकः,
#अन्यथा
    &Compound_Text_दत्त
#यदि_अन्तः
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
स्थिर संरचना {
  ध्रुव वर्ण *name;
  ध्रुव UConverterType type;
} ध्रुव cnvNameType[] = {
#यदि !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#यदि_अन्तः
#यदि !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#यदि_अन्तः
#यदि !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#यदि_अन्तः
#यदि !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#यदि_अन्तः
#यदि !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#यदि_अन्तः
  { "iso88591", UCNV_LATIN_1 },
#यदि !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#यदि_अन्तः
#यदि !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#यदि_अन्तः
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#यदि U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#अन्यथा
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#यदि_अन्तः
#यदि !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#यदि U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#अन्यथा
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#यदि_अन्तः
#यदि_अन्तः
#यदि !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#यदि_अन्तः
  { "utf8", UCNV_UTF8 },
#यदि !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_TEXT}
#यदि_अन्तः
};


/*initializes some global variables */
स्थिर UHashtable *SHARED_दत्त_HASHTABLE = शून्यसूचकः;
स्थिर icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

स्थिर ध्रुव वर्ण **g_उपलब्धम्_Converters = शून्यसूचकः;
स्थिर uint16_t g_उपलब्धम्_Converter_गणनम् = 0;
स्थिर icu::UInitOnce g_उपलब्धम्_Converters_Init_Once {};

#यदि !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
स्थिर वर्ण g_Default_Converter_Name_बफर[UCNV_MAX_CONVERTER_NAME_दीर्घता + 1]; /* +1 for nullptr */
स्थिर ध्रुव वर्ण *gDefaultConverterName = शून्यसूचकः;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
स्थिर ध्रुव UConverter_Shared_दत्त *g_Default_Algorithmic_Shared_दत्त = शून्यसूचकः;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
स्थिर UBool g_Default_Converter_अन्तर्भवति_Option;

#यदि_अन्तः  /* !U_CHARSET_IS_UTF8 */

स्थिर ध्रुव वर्ण दत्त_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
स्थिर शून्य
ucnv_flush_उपलब्धम्_Converter_Cache() {
    g_उपलब्धम्_Converter_गणनम् = 0;
    यदि (g_उपलब्धम्_Converters) {
        uprv_free(const_cast<वर्ण**>(g_उपलब्धम्_Converters));
        g_उपलब्धम्_Converters = शून्यसूचकः;
    }
    g_उपलब्धम्_Converters_Init_Once.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
स्थिर UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    यदि (SHARED_दत्त_HASHTABLE != शून्यसूचकः && uhash_गणनम्(SHARED_दत्त_HASHTABLE) == 0) {
        uhash_close(SHARED_दत्त_HASHTABLE);
        SHARED_दत्त_HASHTABLE = शून्यसूचकः;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flush_उपलब्धम्_Converter_Cache();

#यदि !U_CHARSET_IS_UTF8
    gDefaultConverterName = शून्यसूचकः;
    g_Default_Converter_Name_बफर[0] = 0;
    g_Default_Converter_अन्तर्भवति_Option = असत्य;
    g_Default_Algorithmic_Shared_दत्त = शून्यसूचकः;
#यदि_अन्तः

    प्रत्यागम (SHARED_दत्त_HASHTABLE == शून्यसूचकः);
}

U_CAPI शून्य U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

स्थिर UBool U_CALLCONV
isCnvAcceptable(शून्य * /*context*/,
                ध्रुव वर्ण * /*type*/, ध्रुव वर्ण * /*name*/,
                ध्रुव UDataInfo *pInfo) {
    प्रत्यागम
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->दत्त_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->दत्त_Format[1]==0x6e &&
        pInfo->दत्त_Format[2]==0x76 &&
        pInfo->दत्त_Format[3]==0x74 &&
        pInfo->format_संस्करणम्[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
स्थिर UConverter_Shared_दत्त*
ucnv_दत्त_un_Flatten_Clone(UConverter_आरोपय_तर्काः *p_तर्काः, UData_स्मृतिः *p_दत्त, UError_संहिता *स्थितिः)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ध्रुव uint8_t* raw = static_cast<ध्रुव uint8_t*>(udata_get_स्मृतिः(p_दत्त));
    ध्रुव UConverter_Static_दत्त* source = reinterpret_cast<ध्रुव UConverter_Static_दत्त*>(raw);
    UConverter_Shared_दत्त *दत्त;
    UConverterType type = static_cast<UConverterType>(source->conversionType);

    यदि(U_FAILURE(*स्थितिः))
        प्रत्यागम शून्यसूचकः;

    यदि (static_cast<uint16_t>(type) >= UCNV_सङ्ख्या_सम्बन्धि_SUPPORTED_CONVERTER_TYPES ||
        converter_दत्त[type] == शून्यसूचकः ||
        !converter_दत्त[type]->isReferenceCounted ||
        converter_दत्त[type]->reference_गणक != 1 ||
        source->struct_आकार != परिमाण(UConverter_Static_दत्त))
    {
        *स्थितिः = U_INVALID_सारणी_FORMAT;
        प्रत्यागम शून्यसूचकः;
    }

    दत्त = static_cast<UConverter_Shared_दत्त*>(uprv_malloc(परिमाण(UConverter_Shared_दत्त)));
    यदि(दत्त == शून्यसूचकः) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(दत्त, converter_दत्त[type], परिमाण(UConverter_Shared_दत्त));

    दत्त->static_दत्त = source;

    दत्त->shared_दत्त_Cached = असत्य;

    /* fill in fields from the loaded data */
    दत्त->दत्त_स्मृतिः = (शून्य*)p_दत्त; /* for future use */

    यदि(दत्त->impl->load != शून्यसूचकः) {
        दत्त->impl->load(दत्त, p_तर्काः, raw + source->struct_आकार, स्थितिः);
        यदि(U_FAILURE(*स्थितिः)) {
            uprv_free(दत्त);
            प्रत्यागम शून्यसूचकः;
        }
    }
    प्रत्यागम दत्त;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
स्थिर UConverter_Shared_दत्त *createConverterFromFile(UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता * err)
{
    UData_स्मृतिः *दत्त;
    UConverter_Shared_दत्त *shared_दत्त;

    UTRACE_प्रविष्टिः_OC(UTRACE_UCNV_आरोपय);

    यदि (U_FAILURE (*err)) {
        UTRACE_EXIT_स्थितिः(*err);
        प्रत्यागम शून्यसूचकः;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", p_तर्काः->name, p_तर्काः->pkg);

    दत्त = udata_openChoice(p_तर्काः->pkg, दत्त_TYPE, p_तर्काः->name, isCnvAcceptable, शून्यसूचकः, err);
    यदि(U_FAILURE(*err))
    {
        UTRACE_EXIT_स्थितिः(*err);
        प्रत्यागम शून्यसूचकः;
    }

    shared_दत्त = ucnv_दत्त_un_Flatten_Clone(p_तर्काः, दत्त, err);
    यदि(U_FAILURE(*err))
    {
        udata_close(दत्त);
        UTRACE_EXIT_स्थितिः(*err);
        प्रत्यागम शून्यसूचकः;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_स्थितिः(shared_दत्त, *err);
    प्रत्यागम shared_दत्त;
}

/*returns a converter type from a string
 */
स्थिर ध्रुव UConverter_Shared_दत्त *
getAlgorithmicTypeFromName(ध्रुव वर्ण *realName)
{
    uint32_t mid, start, सीमा;
    uint32_t चरमः_Mid;
    पूर्णाङ्क परिणाम;
    वर्ण strippedName[UCNV_MAX_CONVERTER_NAME_दीर्घता];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    start = 0;
    सीमा = UPRV_LENGTHOF(cnvNameType);
    mid = सीमा;
    चरमः_Mid = UINT32_MAX;

    प्रति (;;) {
        mid = (start + सीमा) / 2;
        यदि (चरमः_Mid == mid) {   /* Have we moved? */
            भङ्ग;  /* We haven't moved, and it wasn't found. */
        }
        चरमः_Mid = mid;
        परिणाम = uprv_strcmp(strippedName, cnvNameType[mid].name);

        यदि (परिणाम < 0) {
            सीमा = mid;
        } अन्यथा यदि (परिणाम > 0) {
            start = mid;
        } अन्यथा {
            प्रत्यागम converter_दत्त[cnvNameType[mid].type];
        }
    }

    प्रत्यागम शून्यसूचकः;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#परिभाषय UCNV_CACHE_आरोपय_गुणकः 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
स्थिर शून्य
ucnv_share_Converter_दत्त(UConverter_Shared_दत्त * दत्त)
{
    UError_संहिता err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    यदि (SHARED_दत्त_HASHTABLE == शून्यसूचकः)
    {
        SHARED_दत्त_HASHTABLE = uhash_open_आकार(uhash_hashChars, uhash_compareChars, शून्यसूचकः,
                            ucnv_io_गणनम्_ज्ञातम्_Converters(&err)*UCNV_CACHE_आरोपय_गुणकः,
                            &err);
        ucnv_enableCleanup();

        यदि (U_FAILURE(err))
            प्रत्यागम;
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
    दत्त->shared_दत्त_Cached = सत्य;

    uhash_put(SHARED_दत्त_HASHTABLE,
            (शून्य*) दत्त->static_दत्त->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            दत्त,
            &err);
    UCNV_DEBUG_LOG("put", दत्त->static_दत्त->name,दत्त);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
स्थिर UConverter_Shared_दत्त *
ucnv_get_Shared_Converter_दत्त(ध्रुव वर्ण *name)
{
    /*special case when no Table has yet been created we return nullptr */
    यदि (SHARED_दत्त_HASHTABLE == शून्यसूचकः)
    {
        प्रत्यागम शून्यसूचकः;
    }
    अन्यथा
    {
        UConverter_Shared_दत्त *rc;

        rc = static_cast<UConverter_Shared_दत्त*>(uhash_get(SHARED_दत्त_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        प्रत्यागम rc;
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
स्थिर UBool
ucnv_delete_Shared_Converter_दत्त(UConverter_Shared_दत्त * dead_Shared_दत्त)
{
    UTRACE_प्रविष्टिः_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_दत्त->static_दत्त->name, dead_Shared_दत्त);

    यदि (dead_Shared_दत्त->reference_गणक > 0) {
        UTRACE_EXIT_मानम्((int32_t)असत्य);
        प्रत्यागम असत्य;
    }

    यदि (dead_Shared_दत्त->impl->unload != शून्यसूचकः) {
        dead_Shared_दत्त->impl->unload(dead_Shared_दत्त);
    }

    यदि(dead_Shared_दत्त->दत्त_स्मृतिः != शून्यसूचकः)
    {
        UData_स्मृतिः *दत्त = (UData_स्मृतिः*)dead_Shared_दत्त->दत्त_स्मृतिः;
        udata_close(दत्त);
    }

    uprv_free(dead_Shared_दत्त);

    UTRACE_EXIT_मानम्((int32_t)सत्य);
    प्रत्यागम सत्य;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_दत्त *
ucnv_आरोपय(UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता *err) {
    UConverter_Shared_दत्त *my_Shared_Converter_दत्त;

    यदि(err == शून्यसूचकः || U_FAILURE(*err)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि(p_तर्काः->pkg != शून्यसूचकः && *p_तर्काः->pkg != 0) {
        /* application-provided converters are not currently cached */
        प्रत्यागम createConverterFromFile(p_तर्काः, err);
    }

    my_Shared_Converter_दत्त = ucnv_get_Shared_Converter_दत्त(p_तर्काः->name);
    यदि (my_Shared_Converter_दत्त == शून्यसूचकः)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_दत्त = createConverterFromFile(p_तर्काः, err);
        यदि (U_FAILURE (*err) || (my_Shared_Converter_दत्त == शून्यसूचकः))
        {
            प्रत्यागम शून्यसूचकः;
        }
        अन्यथा यदि (!p_तर्काः->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_दत्त(my_Shared_Converter_दत्त);
        }
    }
    अन्यथा
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_दत्त->reference_गणक++;
    }

    प्रत्यागम my_Shared_Converter_दत्त;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI शून्य
ucnv_unload(UConverter_Shared_दत्त *shared_दत्त) {
    यदि(shared_दत्त != शून्यसूचकः) {
        यदि (shared_दत्त->reference_गणक > 0) {
            shared_दत्त->reference_गणक--;
        }

        यदि((shared_दत्त->reference_गणक <= 0)&&(shared_दत्त->shared_दत्त_Cached == असत्य)) {
            ucnv_delete_Shared_Converter_दत्त(shared_दत्त);
        }
    }
}

U_CFUNC शून्य
ucnv_unload_Shared_दत्त_If_Ready(UConverter_Shared_दत्त *shared_दत्त)
{
    यदि(shared_दत्त != शून्यसूचकः && shared_दत्त->isReferenceCounted) {
        umtx_तालकम्(&cnvCacheMutex);
        ucnv_unload(shared_दत्त);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC शून्य
ucnv_increment_Ref_गणनम्(UConverter_Shared_दत्त *shared_दत्त)
{
    यदि(shared_दत्त != शून्यसूचकः && shared_दत्त->isReferenceCounted) {
        umtx_तालकम्(&cnvCacheMutex);
        shared_दत्त->reference_गणक++;
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
स्थिर शून्य
parseConverterOptions(ध्रुव वर्ण *अन्तर्_Name,
                      UConverterNamePieces *pPieces,
                      UConverter_आरोपय_तर्काः *p_तर्काः,
                      UError_संहिता *err)
{
    वर्ण *cnvName = pPieces->cnvName;
    वर्ण c;
    int32_t len = 0;

    p_तर्काः->name=अन्तर्_Name;
    p_तर्काः->locale=pPieces->locale;
    p_तर्काः->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    यावत्((c=*अन्तर्_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        यदि (++len>=UCNV_MAX_CONVERTER_NAME_दीर्घता) {
            *err = U_ILLEGAL_तर्कः_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            प्रत्यागम;
        }
        *cnvName++=c;
        अन्तर्_Name++;
    }
    *cnvName=0;
    p_तर्काः->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    यावत्((c=*अन्तर्_Name)!=0) {
        यदि(c==UCNV_OPTION_SEP_CHAR) {
            ++अन्तर्_Name;
        }

        /* inName is behind an option separator */
        यदि(uprv_strncmp(अन्तर्_Name, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            वर्ण *dest=pPieces->locale;

            /* copy the locale option value */
            अन्तर्_Name+=7;
            len=0;
            यावत्((c=*अन्तर्_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++अन्तर्_Name;

                यदि(++len>=ULOC_FULLNAME_क्षमता) {
                    *err=U_ILLEGAL_तर्कः_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    प्रत्यागम;
                }

                *dest++=c;
            }
            *dest=0;
        } अन्यथा यदि(uprv_strncmp(अन्तर्_Name, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            अन्तर्_Name+=8;
            c=*अन्तर्_Name;
            यदि(c==0) {
                p_तर्काः->options=(pPieces->options&=~UCNV_OPTION_संस्करणम्);
                प्रत्यागम;
            } अन्यथा यदि (static_cast<uint8_t>(c - '0') < 10) {
                p_तर्काः->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_संस्करणम्) | static_cast<uint32_t>(c - '0');
                ++अन्तर्_Name;
            }
        } अन्यथा यदि(uprv_strncmp(अन्तर्_Name, "swaplfnl", 8)==0) {
            अन्तर्_Name+=8;
            p_तर्काः->options=(pPieces->options|=UCNV_OPTION_SWAP_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } अन्यथा {
            /* ignore any other options until we define some */
            यावत्(((c = *अन्तर्_Name++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            यदि(c==0) {
                प्रत्यागम;
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
U_CFUNC UConverter_Shared_दत्त *
ucnv_आरोपय_Shared_दत्त(ध्रुव वर्ण *converterName,
                    UConverterNamePieces *pPieces,
                    UConverter_आरोपय_तर्काः *p_तर्काः,
                    UError_संहिता * err) {
    UConverterNamePieces राशिः_Pieces;
    UConverter_आरोपय_तर्काः राशिः_तर्काः;
    UConverter_Shared_दत्त *my_Shared_Converter_दत्त = शून्यसूचकः;
    UError_संहिता आन्तरिकम्_Error_संहिता = U_ZERO_ERROR;
    UBool mayContainOption = सत्य;
    UBool checkForAlgorithmic = सत्य;

    यदि (U_FAILURE (*err)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि(pPieces == शून्यसूचकः) {
        यदि(p_तर्काः != शून्यसूचकः) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_आन्तरिकम्_क्रमादेशः_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        pPieces = &राशिः_Pieces;
    }
    यदि(p_तर्काः == शून्यसूचकः) {
        uprv_memset(&राशिः_तर्काः, 0, परिमाण(राशिः_तर्काः));
        राशिः_तर्काः.size = (int32_t)परिमाण(राशिः_तर्काः);
        p_तर्काः = &राशिः_तर्काः;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    p_तर्काः->name = converterName;
    p_तर्काः->locale = pPieces->locale;
    p_तर्काः->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    यदि (converterName == शून्यसूचकः) {
#यदि U_CHARSET_IS_UTF8
        p_तर्काः->name = "UTF-8";
        प्रत्यागम (UConverter_Shared_दत्त *)converter_दत्त[UCNV_UTF8];
#अन्यथा
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        p_तर्काः->name = ucnv_getDefaultName();
        यदि (p_तर्काः->name == शून्यसूचकः) {
            *err = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        my_Shared_Converter_दत्त = (UConverter_Shared_दत्त *)g_Default_Algorithmic_Shared_दत्त;
        checkForAlgorithmic = असत्य;
        mayContainOption = g_Default_Converter_अन्तर्भवति_Option;
        /* the default converter name is already canonical */
#यदि_अन्तः
    }
    अन्यथा यदि(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        p_तर्काः->name = "UTF-8";
        प्रत्यागम (UConverter_Shared_दत्त *)converter_दत्त[UCNV_UTF8];
    }
    अन्यथा {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, p_तर्काः, err);
        यदि (U_FAILURE(*err)) {
            /* Very bad name used. */
            प्रत्यागम शून्यसूचकः;
        }

        /* get the canonical converter name */
        p_तर्काः->name = ucnv_io_getConverterName(p_तर्काः->name, &mayContainOption, &आन्तरिकम्_Error_संहिता);
        यदि (U_FAILURE(आन्तरिकम्_Error_संहिता) || p_तर्काः->name == शून्यसूचकः) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            p_तर्काः->name = pPieces->cnvName;
        } अन्यथा यदि (आन्तरिकम्_Error_संहिता == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    यदि(mayContainOption && p_तर्काः->name != pPieces->cnvName) {
        parseConverterOptions(p_तर्काः->name, pPieces, p_तर्काः, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    यदि (checkForAlgorithmic) {
        my_Shared_Converter_दत्त = (UConverter_Shared_दत्त *)getAlgorithmicTypeFromName(p_तर्काः->name);
    }
    यदि (my_Shared_Converter_दत्त == शून्यसूचकः)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        p_तर्काः->nestedLoads=1;
        p_तर्काः->pkg=शून्यसूचकः;

        umtx_तालकम्(&cnvCacheMutex);
        my_Shared_Converter_दत्त = ucnv_आरोपय(p_तर्काः, err);
        umtx_unlock(&cnvCacheMutex);
        यदि (U_FAILURE (*err) || (my_Shared_Converter_दत्त == शून्यसूचकः))
        {
            प्रत्यागम शून्यसूचकः;
        }
    }

    प्रत्यागम my_Shared_Converter_दत्त;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ध्रुव वर्ण* converterName,
                     UError_संहिता* err)
{
    UConverterNamePieces राशिः_Pieces;
    UConverter_आरोपय_तर्काः राशिः_तर्काः=UCNV_आरोपय_तर्काः_INITIALIZER;
    UConverter_Shared_दत्त *my_Shared_Converter_दत्त;

    UTRACE_प्रविष्टिः_OC(UTRACE_UCNV_OPEN);

    यदि(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_दत्त = ucnv_आरोपय_Shared_दत्त(converterName, &राशिः_Pieces, &राशिः_तर्काः, err);

        myUConverter = ucnv_create_Converter_From_Shared_दत्त(
            myUConverter, my_Shared_Converter_दत्त,
            &राशिः_तर्काः,
            err);

        यदि(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_स्थितिः(myUConverter, *err);
            प्रत्यागम myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_स्थितिः(*err);
    प्रत्यागम शून्यसूचकः;
}

U_CFUNC UBool
ucnv_canCreateConverter(ध्रुव वर्ण *converterName, UError_संहिता *err) {
    UConverter myUConverter;
    UConverterNamePieces राशिः_Pieces;
    UConverter_आरोपय_तर्काः राशिः_तर्काः=UCNV_आरोपय_तर्काः_INITIALIZER;
    UConverter_Shared_दत्त *my_Shared_Converter_दत्त;

    UTRACE_प्रविष्टिः_OC(UTRACE_UCNV_OPEN);

    यदि(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        राशिः_तर्काः.onlyTestIsLoadable=सत्य;
        my_Shared_Converter_दत्त = ucnv_आरोपय_Shared_दत्त(converterName, &राशिः_Pieces, &राशिः_तर्काः, err);
        ucnv_create_Converter_From_Shared_दत्त(
            &myUConverter, my_Shared_Converter_दत्त,
            &राशिः_तर्काः,
            err);
        ucnv_unload_Shared_दत्त_If_Ready(my_Shared_Converter_दत्त);
    }

    UTRACE_EXIT_स्थितिः(*err);
    प्रत्यागम U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ध्रुव वर्ण* locale,
                                uint32_t options,
                                UError_संहिता* err) {
    UConverter *cnv;
    ध्रुव UConverter_Shared_दत्त *shared_दत्त;
    UConverter_आरोपय_तर्काः राशिः_तर्काः=UCNV_आरोपय_तर्काः_INITIALIZER;

    UTRACE_प्रविष्टिः_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    यदि(type<0 || UCNV_सङ्ख्या_सम्बन्धि_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_तर्कः_ERROR;
        UTRACE_EXIT_स्थितिः(U_ILLEGAL_तर्कः_ERROR);
        प्रत्यागम शून्यसूचकः;
    }

    shared_दत्त = converter_दत्त[type];
    यदि(shared_दत्त == शून्यसूचकः || shared_दत्त->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_तर्कः_ERROR;
        UTRACE_EXIT_स्थितिः(U_ILLEGAL_तर्कः_ERROR);
        प्रत्यागम शून्यसूचकः;
    }

    राशिः_तर्काः.name = "";
    राशिः_तर्काः.options = options;
    राशिः_तर्काः.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_दत्त(
            myUConverter, const_cast<UConverter_Shared_दत्त*>(shared_दत्त),
            &राशिः_तर्काः, err);

    UTRACE_EXIT_PTR_स्थितिः(cnv, *err);
    प्रत्यागम cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ध्रुव वर्ण *packageName, ध्रुव वर्ण *converterName, UError_संहिता * err)
{
    UConverter *myUConverter;
    UConverter_Shared_दत्त *my_Shared_Converter_दत्त;
    UConverterNamePieces राशिः_Pieces;
    UConverter_आरोपय_तर्काः राशिः_तर्काः=UCNV_आरोपय_तर्काः_INITIALIZER;

    UTRACE_प्रविष्टिः_OC(UTRACE_UCNV_OPEN_PACKAGE);

    यदि(U_FAILURE(*err)) {
        UTRACE_EXIT_स्थितिः(*err);
        प्रत्यागम शून्यसूचकः;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    राशिः_Pieces.cnvName[0] = 0;
    राशिः_Pieces.locale[0] = 0;
    राशिः_Pieces.options = 0;
    parseConverterOptions(converterName, &राशिः_Pieces, &राशिः_तर्काः, err);
    यदि (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_स्थितिः(*err);
        प्रत्यागम शून्यसूचकः;
    }
    राशिः_तर्काः.nestedLoads=1;
    राशिः_तर्काः.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_दत्त = createConverterFromFile(&राशिः_तर्काः, err);

    यदि (U_FAILURE(*err)) {
        UTRACE_EXIT_स्थितिः(*err);
        प्रत्यागम शून्यसूचकः;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_दत्त(शून्यसूचकः, my_Shared_Converter_दत्त, &राशिः_तर्काः, err);

    यदि (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_स्थितिः(*err);
        प्रत्यागम शून्यसूचकः;
    }

    UTRACE_EXIT_PTR_स्थितिः(myUConverter, *err);
    प्रत्यागम myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_दत्त(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_दत्त* my_Shared_Converter_दत्त,
                                   UConverter_आरोपय_तर्काः* p_तर्काः,
                                   UError_संहिता* err)
{
    UBool isCopyLocal;

    यदि(U_FAILURE(*err)) {
        ucnv_unload_Shared_दत्त_If_Ready(my_Shared_Converter_दत्त);
        प्रत्यागम myUConverter;
    }
    यदि(myUConverter == शून्यसूचकः)
    {
        myUConverter = (UConverter *) uprv_malloc (परिमाण (UConverter));
        यदि(myUConverter == शून्यसूचकः)
        {
            *err = U_स्मृतिः_आवण्टनम्_ERROR;
            ucnv_unload_Shared_दत्त_If_Ready(my_Shared_Converter_दत्त);
            प्रत्यागम शून्यसूचकः;
        }
        isCopyLocal = असत्य;
    } अन्यथा {
        isCopyLocal = सत्य;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, परिमाण(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_दत्त = my_Shared_Converter_दत्त;
    myUConverter->options = p_तर्काः->options;
    यदि(!p_तर्काः->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_गन्तव्यम्_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->गन्तव्यम्_Unicode_स्थितिः = my_Shared_Converter_दत्त->गन्तव्यम्_Unicode_स्थितिः;
        myUConverter->max_अष्टकानि_प्रत्येकम्_UChar = my_Shared_Converter_दत्त->static_दत्त->max_अष्टकानि_प्रत्येकम्_Char;
        myUConverter->subChar1 = my_Shared_Converter_दत्त->static_दत्त->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_दत्त->static_दत्त->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_दत्त->static_दत्त->subChar, myUConverter->subCharLen);
        myUConverter->गन्तव्यम्_UCallback_Reason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    यदि(my_Shared_Converter_दत्त->impl->उद्घाटय != शून्यसूचकः) {
        my_Shared_Converter_दत्त->impl->उद्घाटय(myUConverter, p_तर्काः, err);
        यदि(U_FAILURE(*err) && !p_तर्काः->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            प्रत्यागम शून्यसूचकः;
        }
    }

    प्रत्यागम myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_दत्त *my_Shared_दत्त = शून्यसूचकः;
    int32_t pos;
    int32_t सारणी_Deleted_अङ्कः = 0;
    ध्रुव UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, remaining;

    UTRACE_प्रविष्टिः_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    यदि (SHARED_दत्त_HASHTABLE == शून्यसूचकः) {
        UTRACE_EXIT_मानम्((int32_t)0);
        प्रत्यागम 0;
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
    umtx_तालकम्(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    कुरु {
        remaining = 0;
        pos = UHASH_FIRST;
        यावत् ((e = uhash_अग्र_Element (SHARED_दत्त_HASHTABLE, &pos)) != शून्यसूचकः)
        {
            my_Shared_दत्त = (UConverter_Shared_दत्त *) e->value.pointer;
            /*deletes only if reference counter == 0 */
            यदि (my_Shared_दत्त->reference_गणक == 0)
            {
                सारणी_Deleted_अङ्कः++;

                UCNV_DEBUG_LOG("del",my_Shared_दत्त->static_दत्त->name,my_Shared_दत्त);

                uhash_removeElement(SHARED_दत्त_HASHTABLE, e);
                my_Shared_दत्त->shared_दत्त_Cached = असत्य;
                ucnv_delete_Shared_Converter_दत्त (my_Shared_दत्त);
            } अन्यथा {
                ++remaining;
            }
        }
    } यावत्(++i == 1 && remaining > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", remaining);

    UTRACE_EXIT_मानम्(सारणी_Deleted_अङ्कः);
    प्रत्यागम सारणी_Deleted_अङ्कः;
}

/* available converters list --------------------------------------------------- */

स्थिर शून्य U_CALLCONV init_उपलब्धम्_Converters_List(UError_संहिता &err_संहिता) {
    U_ASSERT(g_उपलब्धम्_Converter_गणनम् == 0);
    U_ASSERT(g_उपलब्धम्_Converters == शून्यसूचकः);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_open_All_नामानि(&err_संहिता);
    int32_t all_Converter_गणनम् = uenum_गणनम्(allConvEnum, &err_संहिता);
    यदि (U_FAILURE(err_संहिता)) {
        प्रत्यागम;
    }

    /* We can't have more than "*converterTable" converters to open */
    g_उपलब्धम्_Converters = static_cast<ध्रुव वर्ण**>(uprv_malloc(all_Converter_गणनम् * परिमाण(वर्ण*)));
    यदि (!g_उपलब्धम्_Converters) {
        err_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UError_संहिता local_स्थितिः = U_ZERO_ERROR;
    UConverter अस्थायी_Converter;
    ucnv_close(ucnv_createConverter(&अस्थायी_Converter, शून्यसूचकः, &local_स्थितिः));

    g_उपलब्धम्_Converter_गणनम् = 0;

    प्रति (int32_t idx = 0; idx < all_Converter_गणनम्; idx++) {
        local_स्थितिः = U_ZERO_ERROR;
        ध्रुव वर्ण *converterName = uenum_अग्र(allConvEnum, शून्यसूचकः, &local_स्थितिः);
        यदि (ucnv_canCreateConverter(converterName, &local_स्थितिः)) {
            g_उपलब्धम्_Converters[g_उपलब्धम्_Converter_गणनम्++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


स्थिर UBool have_उपलब्धम्_Converter_List(UError_संहिता *p_Error_संहिता) {
    umtx_initOnce(g_उपलब्धम्_Converters_Init_Once, &init_उपलब्धम्_Converters_List, *p_Error_संहिता);
    प्रत्यागम U_SUCCESS(*p_Error_संहिता);
}

U_CFUNC uint16_t
ucnv_bld_गणनम्_उपलब्धम्_Converters(UError_संहिता *p_Error_संहिता) {
    यदि (have_उपलब्धम्_Converter_List(p_Error_संहिता)) {
        प्रत्यागम g_उपलब्धम्_Converter_गणनम्;
    }
    प्रत्यागम 0;
}

U_CFUNC ध्रुव वर्ण *
ucnv_bld_get_उपलब्धम्_Converter(uint16_t n, UError_संहिता *p_Error_संहिता) {
    यदि (have_उपलब्धम्_Converter_List(p_Error_संहिता)) {
        यदि (n < g_उपलब्धम्_Converter_गणनम्) {
            प्रत्यागम g_उपलब्धम्_Converters[n];
        }
        *p_Error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
    }
    प्रत्यागम शून्यसूचकः;
}

/* default converter name --------------------------------------------------- */

#यदि !U_CHARSET_IS_UTF8
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
स्थिर अन्तःस्थ शून्य
आन्तरिकम्_समुच्चयः_Name(ध्रुव वर्ण *name, UError_संहिता *स्थितिः) {
    UConverterNamePieces राशिः_Pieces;
    UConverter_आरोपय_तर्काः राशिः_तर्काः=UCNV_आरोपय_तर्काः_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool अन्तर्भवति_Option = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != शून्यसूचकः);
    ध्रुव UConverter_Shared_दत्त *algorithmic_Shared_दत्त;

    राशिः_तर्काः.name = name;
    यदि(अन्तर्भवति_Option) {
        राशिः_Pieces.cnvName[0] = 0;
        राशिः_Pieces.locale[0] = 0;
        राशिः_Pieces.options = 0;
        parseConverterOptions(name, &राशिः_Pieces, &राशिः_तर्काः, स्थितिः);
        यदि(U_FAILURE(*स्थितिः)) {
            प्रत्यागम;
        }
    }
    algorithmic_Shared_दत्त = getAlgorithmicTypeFromName(राशिः_तर्काः.name);

    umtx_तालकम्(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_दत्त = algorithmic_Shared_दत्त;
    g_Default_Converter_अन्तर्भवति_Option = अन्तर्भवति_Option;
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
#यदि_अन्तः

/*
 * In order to be really thread-safe, the get function would have to take
 * a buffer parameter and copy the current string inside a mutex block.
 * This implementation only tries to be really thread-safe while
 * setting the name.
 * It assumes that setting a pointer is atomic.
 */

U_CAPI ध्रुव वर्ण*  U_EXPORT2
ucnv_getDefaultName() {
#यदि U_CHARSET_IS_UTF8
    प्रत्यागम "UTF-8";
#अन्यथा
    /* local variable to be thread-safe */
    ध्रुव वर्ण *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex तालकम्(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    यदि(name==शून्यसूचकः) {
        UError_संहिता error_संहिता = U_ZERO_ERROR;
        UConverter *cnv = शून्यसूचकः;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        यदि(name != शून्यसूचकः) {
            cnv = ucnv_open(name, &error_संहिता);
            यदि(U_SUCCESS(error_संहिता) && cnv != शून्यसूचकः) {
                name = ucnv_getName(cnv, &error_संहिता);
            }
        }

        यदि(name == शून्यसूचकः || name[0] == 0
            || U_FAILURE(error_संहिता) || cnv == शून्यसूचकः
            || uprv_strlen(name)>=परिमाण(g_Default_Converter_Name_बफर))
        {
            /* Panic time, let's use a fallback. */
#यदि (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#नोचेद्यदि U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_SWAP_LFNL_OPTION_STRING;
#अन्यथा
            name = "ibm-37_P100-1995";
#यदि_अन्तः
        }

        आन्तरिकम्_समुच्चयः_Name(name, &error_संहिता);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    प्रत्यागम name;
#यदि_अन्तः
}

#यदि U_CHARSET_IS_UTF8
U_CAPI शून्य U_EXPORT2 ucnv_समुच्चयः_Default_Name(ध्रुव वर्ण *) {}
#अन्यथा
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI शून्य U_EXPORT2
ucnv_समुच्चयः_Default_Name(ध्रुव वर्ण *converterName) {
    यदि(converterName==शून्यसूचकः) {
        /* reset to the default codepage */
        gDefaultConverterName=शून्यसूचकः;
    } अन्यथा {
        UError_संहिता error_संहिता = U_ZERO_ERROR;
        UConverter *cnv = शून्यसूचकः;
        ध्रुव वर्ण *name = शून्यसूचकः;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &error_संहिता);
        यदि(U_SUCCESS(error_संहिता) && cnv != शून्यसूचकः) {
            name = ucnv_getName(cnv, &error_संहिता);
        }

        यदि(U_SUCCESS(error_संहिता) && name!=शून्यसूचकः) {
            आन्तरिकम्_समुच्चयः_Name(name, &error_संहिता);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#यदि_अन्तः

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#यदि !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_swap(ध्रुव UDataSwapper *ds,
          ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
          UError_संहिता *p_Error_संहिता) {
    ध्रुव UDataInfo *pInfo;
    int32_t शीर्षकम्_आकार;

    ध्रुव uint8_t *अन्तर्_अष्टकानि;
    uint8_t *बहिर्गमनम्_अष्टकानि;

    uint32_t विस्थापनम्, count, static_दत्त_आकार;
    int32_t size;

    ध्रुव UConverter_Static_दत्त *अन्तर्_Static_दत्त;
    UConverter_Static_दत्त *बहिर्गमनम्_Static_दत्त;

    ध्रुव _MBCSHeader *अन्तर्_MBCSHeader;
    _MBCSHeader *बहिर्गमनम्_MBCSHeader;
    _MBCSHeader mbcs_शीर्षकम्;
    uint32_t mbcs_शीर्षकम्_दीर्घता;
    UBool noFromU=असत्य;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_सूचकांक_दीर्घता;

    ध्रुव int32_t *अन्तर्_Ext_Indexes;
    int32_t ext_विस्थापनम्;

    /* udata_swapDataHeader checks the arguments */
    शीर्षकम्_आकार=udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
    यदि(!(
        pInfo->दत्त_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->दत्त_Format[1]==0x6e &&
        pInfo->दत्त_Format[2]==0x76 &&
        pInfo->दत्त_Format[3]==0x74 &&
        pInfo->format_संस्करणम्[0]==6 &&
        pInfo->format_संस्करणम्[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->दत्त_Format[0], pInfo->दत्त_Format[1],
                         pInfo->दत्त_Format[2], pInfo->दत्त_Format[3],
                         pInfo->format_संस्करणम्[0], pInfo->format_संस्करणम्[1]);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_अष्टकानि=(ध्रुव uint8_t *)अन्तर्_दत्त+शीर्षकम्_आकार;
    बहिर्गमनम्_अष्टकानि=(बहिर्गमनम्_दत्त == शून्यसूचकः) ? शून्यसूचकः : (uint8_t *)बहिर्गमनम्_दत्त+शीर्षकम्_आकार;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    अन्तर्_Static_दत्त=(ध्रुव UConverter_Static_दत्त *)अन्तर्_अष्टकानि;
    बहिर्गमनम्_Static_दत्त=(UConverter_Static_दत्त *)बहिर्गमनम्_अष्टकानि;

    यदि(length<0) {
        static_दत्त_आकार=ds->readUInt32(अन्तर्_Static_दत्त->struct_आकार);
    } अन्यथा {
        length-=शीर्षकम्_आकार;
        यदि( length<(int32_t)परिमाण(UConverter_Static_दत्त) ||
            (uint32_t)length<(static_दत्त_आकार=ds->readUInt32(अन्तर्_Static_दत्त->struct_आकार))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    यदि(length>=0) {
        /* swap the static data */
        यदि(अन्तर्_Static_दत्त!=बहिर्गमनम्_Static_दत्त) {
            uprv_memcpy(बहिर्गमनम्_Static_दत्त, अन्तर्_Static_दत्त, static_दत्त_आकार);
        }

        ds->swapArray32(ds, &अन्तर्_Static_दत्त->struct_आकार, 4,
                           &बहिर्गमनम्_Static_दत्त->struct_आकार, p_Error_संहिता);
        ds->swapArray32(ds, &अन्तर्_Static_दत्त->codepage, 4,
                           &बहिर्गमनम्_Static_दत्त->codepage, p_Error_संहिता);

        ds->swapInvChars(ds, अन्तर्_Static_दत्त->name, (int32_t)uprv_strlen(अन्तर्_Static_दत्त->name),
                            बहिर्गमनम्_Static_दत्त->name, p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            प्रत्यागम 0;
        }
    }

    अन्तर्_अष्टकानि+=static_दत्त_आकार;
    यदि (बहिर्गमनम्_अष्टकानि != शून्यसूचकः) बहिर्गमनम्_अष्टकानि+=static_दत्त_आकार;
    यदि(length>=0) {
        length-=(int32_t)static_दत्त_आकार;
    }

    /* check for supported conversionType values */
    यदि(अन्तर्_Static_दत्त->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        अन्तर्_MBCSHeader=(ध्रुव _MBCSHeader *)अन्तर्_अष्टकानि;
        बहिर्गमनम्_MBCSHeader=(_MBCSHeader *)बहिर्गमनम्_अष्टकानि;

        यदि(0<=length && length<(int32_t)परिमाण(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
        यदि(अन्तर्_MBCSHeader->संस्करणम्[0]==4 && अन्तर्_MBCSHeader->संस्करणम्[1]>=1) {
            mbcs_शीर्षकम्_दीर्घता=MBCS_शीर्षकम्_V4_दीर्घता;
        } अन्यथा यदि(अन्तर्_MBCSHeader->संस्करणम्[0]==5 && अन्तर्_MBCSHeader->संस्करणम्[1]>=3 &&
                  ((mbcs_शीर्षकम्.options=ds->readUInt32(अन्तर्_MBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_आवरणम्)==0
        ) {
            mbcs_शीर्षकम्_दीर्घता=mbcs_शीर्षकम्.options&MBCS_OPT_दीर्घता_आवरणम्;
            noFromU = (mbcs_शीर्षकम्.options & MBCS_OPT_NO_FROM_U) != 0;
        } अन्यथा {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             अन्तर्_MBCSHeader->संस्करणम्[0], अन्तर्_MBCSHeader->संस्करणम्[1]);
            *p_Error_संहिता=U_UNSUPPORTED_ERROR;
            प्रत्यागम 0;
        }

        uprv_memcpy(mbcs_शीर्षकम्.संस्करणम्, अन्तर्_MBCSHeader->संस्करणम्, 4);
        mbcs_शीर्षकम्.गणनम्_अवस्थाः=         ds->readUInt32(अन्तर्_MBCSHeader->गणनम्_अवस्थाः);
        mbcs_शीर्षकम्.गणनम्_गन्तव्यम्_UFallbacks=   ds->readUInt32(अन्तर्_MBCSHeader->गणनम्_गन्तव्यम्_UFallbacks);
        mbcs_शीर्षकम्.विस्थापनम्_गन्तव्यम्_UCode_Units=  ds->readUInt32(अन्तर्_MBCSHeader->विस्थापनम्_गन्तव्यम्_UCode_Units);
        mbcs_शीर्षकम्.विस्थापनम्_From_UTable=    ds->readUInt32(अन्तर्_MBCSHeader->विस्थापनम्_From_UTable);
        mbcs_शीर्षकम्.विस्थापनम्_From_UBytes=    ds->readUInt32(अन्तर्_MBCSHeader->विस्थापनम्_From_UBytes);
        mbcs_शीर्षकम्.flags=               ds->readUInt32(अन्तर्_MBCSHeader->flags);
        mbcs_शीर्षकम्.from_UBytes_दीर्घता=    ds->readUInt32(अन्तर्_MBCSHeader->from_UBytes_दीर्घता);
        /* mbcsHeader.options have been read above */

        ext_विस्थापनम्=(int32_t)(mbcs_शीर्षकम्.flags>>8);
        outputType=(uint8_t)mbcs_शीर्षकम्.flags;
        यदि(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *p_Error_संहिता=U_UNSUPPORTED_ERROR;
            प्रत्यागम 0;
        }

        /* make sure that the output type is known */
        विकल्प(outputType) {
        पक्ष MBCS_OUTPUT_1:
        पक्ष MBCS_OUTPUT_2:
        पक्ष MBCS_OUTPUT_3:
        पक्ष MBCS_OUTPUT_4:
        पक्ष MBCS_OUTPUT_3_EUC:
        पक्ष MBCS_OUTPUT_4_EUC:
        पक्ष MBCS_OUTPUT_2_SISO:
        पक्ष MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            भङ्ग;
        स्वभाव:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *p_Error_संहिता=U_UNSUPPORTED_ERROR;
            प्रत्यागम 0;
        }

        /* calculate the length of the MBCS data */

        /*
         * utf8Friendly MBCS files (mbcsHeader.version 4.3)
         * contain an additional mbcsIndex table:
         *   uint16_t[(maxFastUChar+1)>>6];
         * where maxFastUChar=((mbcsHeader.version[2]<<8)|0xff).
         */
        maxFastUChar=0;
        mbcs_सूचकांक_दीर्घता=0;
        यदि( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcs_शीर्षकम्.संस्करणम्[1]>=3 && (maxFastUChar=mbcs_शीर्षकम्.संस्करणम्[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_सूचकांक_दीर्घता=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        यदि(ext_विस्थापनम्==0) {
            size=(int32_t)(mbcs_शीर्षकम्.विस्थापनम्_From_UBytes+mbcs_सूचकांक_दीर्घता);
            यदि(!noFromU) {
                size+=(int32_t)mbcs_शीर्षकम्.from_UBytes_दीर्घता;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            अन्तर्_Ext_Indexes=शून्यसूचकः;
        } अन्यथा {
            /* there is extension data after the base data, see ucnv_ext.h */
            यदि(length>=0 && length<(ext_विस्थापनम्+UCNV_EXT_INDEXES_MIN_दीर्घता*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }

            अन्तर्_Ext_Indexes=(ध्रुव int32_t *)(अन्तर्_अष्टकानि+ext_विस्थापनम्);
            size=ext_विस्थापनम्+udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_आकार]);
        }

        यदि(length>=0) {
            यदि(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }

            /* copy the data for inaccessible bytes */
            यदि(अन्तर्_अष्टकानि!=बहिर्गमनम्_अष्टकानि) {
                uprv_memcpy(बहिर्गमनम्_अष्टकानि, अन्तर्_अष्टकानि, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_शीर्षकम्_दीर्घता*4;
            ds->swapArray32(ds, &अन्तर्_MBCSHeader->गणनम्_अवस्थाः, count-4,
                               &बहिर्गमनम्_MBCSHeader->गणनम्_अवस्थाः, p_Error_संहिता);

            यदि(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ध्रुव वर्ण *अन्तर्_आधारः_Name=(ध्रुव वर्ण *)अन्तर्_अष्टकानि+count;
                वर्ण *बहिर्गमनम्_आधारः_Name=(वर्ण *)बहिर्गमनम्_अष्टकानि+count;
                ds->swapInvChars(ds, अन्तर्_आधारः_Name, (int32_t)uprv_strlen(अन्तर्_आधारः_Name),
                                    बहिर्गमनम्_आधारः_Name, p_Error_संहिता);
            } अन्यथा {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                विस्थापनम्=count;
                count=mbcs_शीर्षकम्.गणनम्_अवस्थाः*1024;
                ds->swapArray32(ds, अन्तर्_अष्टकानि+विस्थापनम्, (int32_t)count,
                                   बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* swap the toUFallbacks[] */
                विस्थापनम्+=count;
                count=mbcs_शीर्षकम्.गणनम्_गन्तव्यम्_UFallbacks*8;
                ds->swapArray32(ds, अन्तर्_अष्टकानि+विस्थापनम्, (int32_t)count,
                                   बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* swap the unicodeCodeUnits[] */
                विस्थापनम्=mbcs_शीर्षकम्.विस्थापनम्_गन्तव्यम्_UCode_Units;
                count=mbcs_शीर्षकम्.विस्थापनम्_From_UTable-विस्थापनम्;
                ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, (int32_t)count,
                                   बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* offset to the stage 1 table, independent of the outputType */
                विस्थापनम्=mbcs_शीर्षकम्.विस्थापनम्_From_UTable;

                यदि(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcs_शीर्षकम्.विस्थापनम्_From_UBytes-विस्थापनम्)+mbcs_शीर्षकम्.from_UBytes_दीर्घता;
                    ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, (int32_t)count,
                                       बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);
                } अन्यथा {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    यदि(अन्तर्_Static_दत्त->unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } अन्यथा {
                        count=0x40*2; /* only BMP */
                    }
                    ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, (int32_t)count,
                                       बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                    /* stage 2 table: uint32_t[] */
                    विस्थापनम्+=count;
                    count=mbcs_शीर्षकम्.विस्थापनम्_From_UBytes-विस्थापनम्;
                    ds->swapArray32(ds, अन्तर्_अष्टकानि+विस्थापनम्, (int32_t)count,
                                       बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    विस्थापनम्=mbcs_शीर्षकम्.विस्थापनम्_From_UBytes;
                    count= noFromU ? 0 : mbcs_शीर्षकम्.from_UBytes_दीर्घता;
                    विकल्प(outputType) {
                    पक्ष MBCS_OUTPUT_2:
                    पक्ष MBCS_OUTPUT_3_EUC:
                    पक्ष MBCS_OUTPUT_2_SISO:
                        ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, (int32_t)count,
                                           बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);
                        भङ्ग;
                    पक्ष MBCS_OUTPUT_4:
                        ds->swapArray32(ds, अन्तर्_अष्टकानि+विस्थापनम्, (int32_t)count,
                                           बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);
                        भङ्ग;
                    स्वभाव:
                        /* just uint8_t[], nothing to swap */
                        भङ्ग;
                    }

                    यदि(mbcs_सूचकांक_दीर्घता!=0) {
                        विस्थापनम्+=count;
                        count=mbcs_सूचकांक_दीर्घता;
                        ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, (int32_t)count,
                                           बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);
                    }
                }
            }

            यदि(ext_विस्थापनम्!=0) {
                /* swap the extension data */
                अन्तर्_अष्टकानि+=ext_विस्थापनम्;
                बहिर्गमनम्_अष्टकानि+=ext_विस्थापनम्;

                /* swap toUTable[] */
                विस्थापनम्=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_गन्तव्यम्_U_सूचकांक]);
                length=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_गन्तव्यम्_U_दीर्घता]);
                ds->swapArray32(ds, अन्तर्_अष्टकानि+विस्थापनम्, length*4, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* swap toUUChars[] */
                विस्थापनम्=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_गन्तव्यम्_U_UCHARS_सूचकांक]);
                length=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_गन्तव्यम्_U_UCHARS_दीर्घता]);
                ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, length*2, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* swap fromUTableUChars[] */
                विस्थापनम्=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_FROM_U_UCHARS_सूचकांक]);
                length=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_FROM_U_दीर्घता]);
                ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, length*2, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* swap fromUTableValues[] */
                विस्थापनम्=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_FROM_U_मानानि_सूचकांक]);
                /* same length as for fromUTableUChars[] */
                ds->swapArray32(ds, अन्तर्_अष्टकानि+विस्थापनम्, length*4, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                विस्थापनम्=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_सूचकांक]);
                length=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_दीर्घता]);
                ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, length*2, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* swap fromUStage3[] */
                विस्थापनम्=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_सूचकांक]);
                length=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_दीर्घता]);
                ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, length*2, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* swap fromUStage3b[] */
                विस्थापनम्=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_सूचकांक]);
                length=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_दीर्घता]);
                ds->swapArray32(ds, अन्तर्_अष्टकानि+विस्थापनम्, length*4, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);

                /* swap indexes[] */
                length=udata_readInt32(ds, अन्तर्_Ext_Indexes[UCNV_EXT_INDEXES_दीर्घता]);
                ds->swapArray32(ds, अन्तर्_अष्टकानि, length*4, बहिर्गमनम्_अष्टकानि, p_Error_संहिता);
            }
        }
    } अन्यथा {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         अन्तर्_Static_दत्त->conversionType);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    प्रत्यागम शीर्षकम्_आकार+(int32_t)static_दत्त_आकार+size;
}

#यदि_अन्तः /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#यदि_अन्तः
