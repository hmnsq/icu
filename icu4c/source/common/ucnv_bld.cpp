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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/putil.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/ucnv.h"
#ضمن "unicode/uloc.h"
#ضمن "mutex.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"
#ضمن "utracimp.h"
#ضمن "ucnv_io.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnvmbcs.h"
#ضمن "ucnv_ext.h"
#ضمن "ucnv_cnv.h"
#ضمن "ucnv_imp.h"
#ضمن "uhash.h"
#ضمن "umutex.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "ucln_cmn.h"
#ضمن "ustr_cnv.h"


#إذا 0
#ضمن <stdio.h>
خارجي فراغ UCNV_DEBUG_LOG(حرف *what, حرف *who, فراغ *p, صحيح l);
#وصف_ڪريو UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#وإلا
# define UCNV_DEBUG_LOG(x,y,z)
#جيڪڏهن_پورو

ساكن ثابت UConverter_Shared_معلومات * ثابت
converter_معلومات[UCNV_عدد_جو_SUPPORTED_CONVERTER_TYPES]={
    خالي_اشارو, خالي_اشارو,

#إذا UCONFIG_NO_LEGACY_CONVERSION
    خالي_اشارو,
#وإلا
    &_MBCSData,
#جيڪڏهن_پورو

    &Latin1_معلومات,
    &UTF8_معلومات, &_UTF16BEData, &_UTF16LEData,
#إذا UCONFIG_ONLY_HTML_CONVERSION
    خالي_اشارو, خالي_اشارو,
#وإلا
    &_UTF32BEData, &_UTF32LEData,
#جيڪڏهن_پورو
    خالي_اشارو,

#إذا UCONFIG_NO_LEGACY_CONVERSION
    خالي_اشارو,
#وإلا
    &ISO2022_معلومات,
#جيڪڏهن_پورو

#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو,
    خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو,
    خالي_اشارو,
#وإلا
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#جيڪڏهن_پورو

#إذا UCONFIG_ONLY_HTML_CONVERSION
    خالي_اشارو,
#وإلا
    &_SCSUData,
#جيڪڏهن_پورو


#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    خالي_اشارو,
#وإلا
    &_ISCIIData,
#جيڪڏهن_پورو

    &_ASCIIData,
#إذا UCONFIG_ONLY_HTML_CONVERSION
    خالي_اشارو, خالي_اشارو, &UTF16_معلومات, خالي_اشارو, خالي_اشارو, خالي_اشارو,
#وإلا
    &UTF7_معلومات, &Bocu1_معلومات, &UTF16_معلومات, &UTF32_معلومات, &CESU8_معلومات, &_IMAPData,
#جيڪڏهن_پورو

#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    خالي_اشارو,
#وإلا
    &Compound_متن_معلومات
#جيڪڏهن_پورو
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
ساكن بنية {
  ثابت حرف *name;
  ثابت UConverterType type;
} ثابت cnvNameType[] = {
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#جيڪڏهن_پورو
#إذا !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#جيڪڏهن_پورو
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#جيڪڏهن_پورو
#إذا !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#جيڪڏهن_پورو
#إذا !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#جيڪڏهن_پورو
  { "iso88591", UCNV_LATIN_1 },
#إذا !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#جيڪڏهن_پورو
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#جيڪڏهن_پورو
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#إذا U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#وإلا
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#جيڪڏهن_پورو
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#إذا U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#وإلا
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#جيڪڏهن_پورو
#جيڪڏهن_پورو
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#جيڪڏهن_پورو
  { "utf8", UCNV_UTF8 },
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_متن}
#جيڪڏهن_پورو
};


/*initializes some global variables */
ساكن UHashtable *SHARED_معلومات_HASHTABLE = خالي_اشارو;
ساكن icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

ساكن ثابت حرف **g_موجود_Converters = خالي_اشارو;
ساكن uint16_t g_موجود_Converter_ڳڻپ = 0;
ساكن icu::UInitOnce g_موجود_Converters_Init_Once {};

#إذا !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
ساكن حرف g_Default_Converter_Name_بفر[UCNV_MAX_CONVERTER_NAME_ڊيگهائي + 1]; /* +1 for nullptr */
ساكن ثابت حرف *gDefaultConverterName = خالي_اشارو;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
ساكن ثابت UConverter_Shared_معلومات *g_Default_Algorithmic_Shared_معلومات = خالي_اشارو;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
ساكن UBool g_Default_Converter_شامل_آهي_Option;

#جيڪڏهن_پورو  /* !U_CHARSET_IS_UTF8 */

ساكن ثابت حرف معلومات_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
ساكن فراغ
ucnv_flush_موجود_Converter_Cache() {
    g_موجود_Converter_ڳڻپ = 0;
    إذا (g_موجود_Converters) {
        uprv_free(const_cast<حرف**>(g_موجود_Converters));
        g_موجود_Converters = خالي_اشارو;
    }
    g_موجود_Converters_Init_Once.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
ساكن UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    إذا (SHARED_معلومات_HASHTABLE != خالي_اشارو && uhash_ڳڻپ(SHARED_معلومات_HASHTABLE) == 0) {
        uhash_close(SHARED_معلومات_HASHTABLE);
        SHARED_معلومات_HASHTABLE = خالي_اشارو;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flush_موجود_Converter_Cache();

#إذا !U_CHARSET_IS_UTF8
    gDefaultConverterName = خالي_اشارو;
    g_Default_Converter_Name_بفر[0] = 0;
    g_Default_Converter_شامل_آهي_Option = خطأ;
    g_Default_Algorithmic_Shared_معلومات = خالي_اشارو;
#جيڪڏهن_پورو

    أرجع (SHARED_معلومات_HASHTABLE == خالي_اشارو);
}

U_CAPI فراغ U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

ساكن UBool U_CALLCONV
isCnvAcceptable(فراغ * /*context*/,
                ثابت حرف * /*type*/, ثابت حرف * /*name*/,
                ثابت UDataInfo *pInfo) {
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->معلومات_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->معلومات_Format[1]==0x6e &&
        pInfo->معلومات_Format[2]==0x76 &&
        pInfo->معلومات_Format[3]==0x74 &&
        pInfo->format_نسخو[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
ساكن UConverter_Shared_معلومات*
ucnv_معلومات_un_Flatten_Clone(UConverter_ڀريو_دليلون *p_دليلون, UData_يادگيري *p_معلومات, UError_ڪوڊ *حالت)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ثابت uint8_t* raw = static_cast<ثابت uint8_t*>(udata_get_يادگيري(p_معلومات));
    ثابت UConverter_Static_معلومات* ذريعو = reinterpret_cast<ثابت UConverter_Static_معلومات*>(raw);
    UConverter_Shared_معلومات *معلومات;
    UConverterType type = static_cast<UConverterType>(ذريعو->conversionType);

    إذا(U_FAILURE(*حالت))
        أرجع خالي_اشارو;

    إذا (static_cast<uint16_t>(type) >= UCNV_عدد_جو_SUPPORTED_CONVERTER_TYPES ||
        converter_معلومات[type] == خالي_اشارو ||
        !converter_معلومات[type]->isReferenceCounted ||
        converter_معلومات[type]->referenceCounter != 1 ||
        ذريعو->struct_سائيز != حجم(UConverter_Static_معلومات))
    {
        *حالت = U_INVALID_جدول_FORMAT;
        أرجع خالي_اشارو;
    }

    معلومات = static_cast<UConverter_Shared_معلومات*>(uprv_malloc(حجم(UConverter_Shared_معلومات)));
    إذا(معلومات == خالي_اشارو) {
        *حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(معلومات, converter_معلومات[type], حجم(UConverter_Shared_معلومات));

    معلومات->static_معلومات = ذريعو;

    معلومات->shared_معلومات_Cached = خطأ;

    /* fill in fields from the loaded data */
    معلومات->معلومات_يادگيري = (فراغ*)p_معلومات; /* for future use */

    إذا(معلومات->impl->load != خالي_اشارو) {
        معلومات->impl->load(معلومات, p_دليلون, raw + ذريعو->struct_سائيز, حالت);
        إذا(U_FAILURE(*حالت)) {
            uprv_free(معلومات);
            أرجع خالي_اشارو;
        }
    }
    أرجع معلومات;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
ساكن UConverter_Shared_معلومات *createConverterFromFile(UConverter_ڀريو_دليلون *p_دليلون, UError_ڪوڊ * err)
{
    UData_يادگيري *معلومات;
    UConverter_Shared_معلومات *shared_معلومات;

    UTRACE_داخلا_OC(UTRACE_UCNV_ڀريو);

    إذا (U_FAILURE (*err)) {
        UTRACE_EXIT_حالت(*err);
        أرجع خالي_اشارو;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", p_دليلون->name, p_دليلون->pkg);

    معلومات = udata_openChoice(p_دليلون->pkg, معلومات_TYPE, p_دليلون->name, isCnvAcceptable, خالي_اشارو, err);
    إذا(U_FAILURE(*err))
    {
        UTRACE_EXIT_حالت(*err);
        أرجع خالي_اشارو;
    }

    shared_معلومات = ucnv_معلومات_un_Flatten_Clone(p_دليلون, معلومات, err);
    إذا(U_FAILURE(*err))
    {
        udata_close(معلومات);
        UTRACE_EXIT_حالت(*err);
        أرجع خالي_اشارو;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_حالت(shared_معلومات, *err);
    أرجع shared_معلومات;
}

/*returns a converter type from a string
 */
ساكن ثابت UConverter_Shared_معلومات *
getAlgorithmicTypeFromName(ثابت حرف *realName)
{
    uint32_t mid, شروعات, حد_بندي;
    uint32_t آخري_Mid;
    صحيح نتيجو;
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_ڊيگهائي];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    شروعات = 0;
    حد_بندي = UPRV_LENGTHOF(cnvNameType);
    mid = حد_بندي;
    آخري_Mid = UINT32_MAX;

    لكل (;;) {
        mid = (شروعات + حد_بندي) / 2;
        إذا (آخري_Mid == mid) {   /* Have we moved? */
            اكسر;  /* We haven't moved, and it wasn't found. */
        }
        آخري_Mid = mid;
        نتيجو = uprv_strcmp(strippedName, cnvNameType[mid].name);

        إذا (نتيجو < 0) {
            حد_بندي = mid;
        } وإلا إذا (نتيجو > 0) {
            شروعات = mid;
        } وإلا {
            أرجع converter_معلومات[cnvNameType[mid].type];
        }
    }

    أرجع خالي_اشارو;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#وصف_ڪريو UCNV_CACHE_ڀريو_ضرب_ڪندڙ 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
ساكن فراغ
ucnv_share_Converter_معلومات(UConverter_Shared_معلومات * معلومات)
{
    UError_ڪوڊ err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    إذا (SHARED_معلومات_HASHTABLE == خالي_اشارو)
    {
        SHARED_معلومات_HASHTABLE = uhash_open_سائيز(uhash_hashChars, uhash_compareChars, خالي_اشارو,
                            ucnv_io_ڳڻپ_معلوم_Converters(&err)*UCNV_CACHE_ڀريو_ضرب_ڪندڙ,
                            &err);
        ucnv_enableCleanup();

        إذا (U_FAILURE(err))
            أرجع;
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
    معلومات->shared_معلومات_Cached = سچ;

    uhash_put(SHARED_معلومات_HASHTABLE,
            (فراغ*) معلومات->static_معلومات->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            معلومات,
            &err);
    UCNV_DEBUG_LOG("put", معلومات->static_معلومات->name,معلومات);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
ساكن UConverter_Shared_معلومات *
ucnv_get_Shared_Converter_معلومات(ثابت حرف *name)
{
    /*special case when no Table has yet been created we return nullptr */
    إذا (SHARED_معلومات_HASHTABLE == خالي_اشارو)
    {
        أرجع خالي_اشارو;
    }
    وإلا
    {
        UConverter_Shared_معلومات *rc;

        rc = static_cast<UConverter_Shared_معلومات*>(uhash_get(SHARED_معلومات_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        أرجع rc;
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
ساكن UBool
ucnv_delete_Shared_Converter_معلومات(UConverter_Shared_معلومات * dead_Shared_معلومات)
{
    UTRACE_داخلا_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_معلومات->static_معلومات->name, dead_Shared_معلومات);

    إذا (dead_Shared_معلومات->referenceCounter > 0) {
        UTRACE_EXIT_قدر((int32_t)خطأ);
        أرجع خطأ;
    }

    إذا (dead_Shared_معلومات->impl->unload != خالي_اشارو) {
        dead_Shared_معلومات->impl->unload(dead_Shared_معلومات);
    }

    إذا(dead_Shared_معلومات->معلومات_يادگيري != خالي_اشارو)
    {
        UData_يادگيري *معلومات = (UData_يادگيري*)dead_Shared_معلومات->معلومات_يادگيري;
        udata_close(معلومات);
    }

    uprv_free(dead_Shared_معلومات);

    UTRACE_EXIT_قدر((int32_t)سچ);
    أرجع سچ;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_معلومات *
ucnv_ڀريو(UConverter_ڀريو_دليلون *p_دليلون, UError_ڪوڊ *err) {
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات;

    إذا(err == خالي_اشارو || U_FAILURE(*err)) {
        أرجع خالي_اشارو;
    }

    إذا(p_دليلون->pkg != خالي_اشارو && *p_دليلون->pkg != 0) {
        /* application-provided converters are not currently cached */
        أرجع createConverterFromFile(p_دليلون, err);
    }

    my_Shared_Converter_معلومات = ucnv_get_Shared_Converter_معلومات(p_دليلون->name);
    إذا (my_Shared_Converter_معلومات == خالي_اشارو)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_معلومات = createConverterFromFile(p_دليلون, err);
        إذا (U_FAILURE (*err) || (my_Shared_Converter_معلومات == خالي_اشارو))
        {
            أرجع خالي_اشارو;
        }
        وإلا إذا (!p_دليلون->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_معلومات(my_Shared_Converter_معلومات);
        }
    }
    وإلا
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_معلومات->referenceCounter++;
    }

    أرجع my_Shared_Converter_معلومات;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI فراغ
ucnv_unload(UConverter_Shared_معلومات *shared_معلومات) {
    إذا(shared_معلومات != خالي_اشارو) {
        إذا (shared_معلومات->referenceCounter > 0) {
            shared_معلومات->referenceCounter--;
        }

        إذا((shared_معلومات->referenceCounter <= 0)&&(shared_معلومات->shared_معلومات_Cached == خطأ)) {
            ucnv_delete_Shared_Converter_معلومات(shared_معلومات);
        }
    }
}

U_CFUNC فراغ
ucnv_unload_Shared_معلومات_If_Ready(UConverter_Shared_معلومات *shared_معلومات)
{
    إذا(shared_معلومات != خالي_اشارو && shared_معلومات->isReferenceCounted) {
        umtx_تالو(&cnvCacheMutex);
        ucnv_unload(shared_معلومات);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC فراغ
ucnv_increment_Ref_ڳڻپ(UConverter_Shared_معلومات *shared_معلومات)
{
    إذا(shared_معلومات != خالي_اشارو && shared_معلومات->isReferenceCounted) {
        umtx_تالو(&cnvCacheMutex);
        shared_معلومات->referenceCounter++;
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
ساكن فراغ
parseConverterOptions(ثابت حرف *اندريون_Name,
                      UConverterNamePieces *pPieces,
                      UConverter_ڀريو_دليلون *p_دليلون,
                      UError_ڪوڊ *err)
{
    حرف *cnvName = pPieces->cnvName;
    حرف c;
    int32_t len = 0;

    p_دليلون->name=اندريون_Name;
    p_دليلون->locale=pPieces->locale;
    p_دليلون->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    طالما((c=*اندريون_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        إذا (++len>=UCNV_MAX_CONVERTER_NAME_ڊيگهائي) {
            *err = U_ILLEGAL_دليل_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            أرجع;
        }
        *cnvName++=c;
        اندريون_Name++;
    }
    *cnvName=0;
    p_دليلون->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    طالما((c=*اندريون_Name)!=0) {
        إذا(c==UCNV_OPTION_SEP_CHAR) {
            ++اندريون_Name;
        }

        /* inName is behind an option separator */
        إذا(uprv_strncmp(اندريون_Name, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            حرف *dest=pPieces->locale;

            /* copy the locale option value */
            اندريون_Name+=7;
            len=0;
            طالما((c=*اندريون_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++اندريون_Name;

                إذا(++len>=ULOC_FULLNAME_صلاحيت) {
                    *err=U_ILLEGAL_دليل_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    أرجع;
                }

                *dest++=c;
            }
            *dest=0;
        } وإلا إذا(uprv_strncmp(اندريون_Name, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            اندريون_Name+=8;
            c=*اندريون_Name;
            إذا(c==0) {
                p_دليلون->options=(pPieces->options&=~UCNV_OPTION_نسخو);
                أرجع;
            } وإلا إذا (static_cast<uint8_t>(c - '0') < 10) {
                p_دليلون->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_نسخو) | static_cast<uint32_t>(c - '0');
                ++اندريون_Name;
            }
        } وإلا إذا(uprv_strncmp(اندريون_Name, "swaplfnl", 8)==0) {
            اندريون_Name+=8;
            p_دليلون->options=(pPieces->options|=UCNV_OPTION_مٽا_سٽا_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } وإلا {
            /* ignore any other options until we define some */
            طالما(((c = *اندريون_Name++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            إذا(c==0) {
                أرجع;
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
U_CFUNC UConverter_Shared_معلومات *
ucnv_ڀريو_Shared_معلومات(ثابت حرف *converterName,
                    UConverterNamePieces *pPieces,
                    UConverter_ڀريو_دليلون *p_دليلون,
                    UError_ڪوڊ * err) {
    UConverterNamePieces ڍير_Pieces;
    UConverter_ڀريو_دليلون ڍير_دليلون;
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات = خالي_اشارو;
    UError_ڪوڊ اندروني_Error_ڪوڊ = U_ZERO_ERROR;
    UBool mayContainOption = سچ;
    UBool checkForAlgorithmic = سچ;

    إذا (U_FAILURE (*err)) {
        أرجع خالي_اشارو;
    }

    إذا(pPieces == خالي_اشارو) {
        إذا(p_دليلون != خالي_اشارو) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_اندروني_پروگرام_ERROR;
            أرجع خالي_اشارو;
        }
        pPieces = &ڍير_Pieces;
    }
    إذا(p_دليلون == خالي_اشارو) {
        uprv_memset(&ڍير_دليلون, 0, حجم(ڍير_دليلون));
        ڍير_دليلون.size = (int32_t)حجم(ڍير_دليلون);
        p_دليلون = &ڍير_دليلون;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    p_دليلون->name = converterName;
    p_دليلون->locale = pPieces->locale;
    p_دليلون->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    إذا (converterName == خالي_اشارو) {
#إذا U_CHARSET_IS_UTF8
        p_دليلون->name = "UTF-8";
        أرجع (UConverter_Shared_معلومات *)converter_معلومات[UCNV_UTF8];
#وإلا
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        p_دليلون->name = ucnv_getDefaultName();
        إذا (p_دليلون->name == خالي_اشارو) {
            *err = U_MISSING_RESOURCE_ERROR;
            أرجع خالي_اشارو;
        }
        my_Shared_Converter_معلومات = (UConverter_Shared_معلومات *)g_Default_Algorithmic_Shared_معلومات;
        checkForAlgorithmic = خطأ;
        mayContainOption = g_Default_Converter_شامل_آهي_Option;
        /* the default converter name is already canonical */
#جيڪڏهن_پورو
    }
    وإلا إذا(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        p_دليلون->name = "UTF-8";
        أرجع (UConverter_Shared_معلومات *)converter_معلومات[UCNV_UTF8];
    }
    وإلا {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, p_دليلون, err);
        إذا (U_FAILURE(*err)) {
            /* Very bad name used. */
            أرجع خالي_اشارو;
        }

        /* get the canonical converter name */
        p_دليلون->name = ucnv_io_getConverterName(p_دليلون->name, &mayContainOption, &اندروني_Error_ڪوڊ);
        إذا (U_FAILURE(اندروني_Error_ڪوڊ) || p_دليلون->name == خالي_اشارو) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            p_دليلون->name = pPieces->cnvName;
        } وإلا إذا (اندروني_Error_ڪوڊ == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    إذا(mayContainOption && p_دليلون->name != pPieces->cnvName) {
        parseConverterOptions(p_دليلون->name, pPieces, p_دليلون, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    إذا (checkForAlgorithmic) {
        my_Shared_Converter_معلومات = (UConverter_Shared_معلومات *)getAlgorithmicTypeFromName(p_دليلون->name);
    }
    إذا (my_Shared_Converter_معلومات == خالي_اشارو)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        p_دليلون->nestedLoads=1;
        p_دليلون->pkg=خالي_اشارو;

        umtx_تالو(&cnvCacheMutex);
        my_Shared_Converter_معلومات = ucnv_ڀريو(p_دليلون, err);
        umtx_unlock(&cnvCacheMutex);
        إذا (U_FAILURE (*err) || (my_Shared_Converter_معلومات == خالي_اشارو))
        {
            أرجع خالي_اشارو;
        }
    }

    أرجع my_Shared_Converter_معلومات;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ثابت حرف* converterName,
                     UError_ڪوڊ* err)
{
    UConverterNamePieces ڍير_Pieces;
    UConverter_ڀريو_دليلون ڍير_دليلون=UCNV_ڀريو_دليلون_INITIALIZER;
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات;

    UTRACE_داخلا_OC(UTRACE_UCNV_OPEN);

    إذا(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_معلومات = ucnv_ڀريو_Shared_معلومات(converterName, &ڍير_Pieces, &ڍير_دليلون, err);

        myUConverter = ucnv_create_Converter_From_Shared_معلومات(
            myUConverter, my_Shared_Converter_معلومات,
            &ڍير_دليلون,
            err);

        إذا(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_حالت(myUConverter, *err);
            أرجع myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_حالت(*err);
    أرجع خالي_اشارو;
}

U_CFUNC UBool
ucnv_canCreateConverter(ثابت حرف *converterName, UError_ڪوڊ *err) {
    UConverter myUConverter;
    UConverterNamePieces ڍير_Pieces;
    UConverter_ڀريو_دليلون ڍير_دليلون=UCNV_ڀريو_دليلون_INITIALIZER;
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات;

    UTRACE_داخلا_OC(UTRACE_UCNV_OPEN);

    إذا(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ڍير_دليلون.onlyTestIsLoadable=سچ;
        my_Shared_Converter_معلومات = ucnv_ڀريو_Shared_معلومات(converterName, &ڍير_Pieces, &ڍير_دليلون, err);
        ucnv_create_Converter_From_Shared_معلومات(
            &myUConverter, my_Shared_Converter_معلومات,
            &ڍير_دليلون,
            err);
        ucnv_unload_Shared_معلومات_If_Ready(my_Shared_Converter_معلومات);
    }

    UTRACE_EXIT_حالت(*err);
    أرجع U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ثابت حرف* locale,
                                uint32_t options,
                                UError_ڪوڊ* err) {
    UConverter *cnv;
    ثابت UConverter_Shared_معلومات *shared_معلومات;
    UConverter_ڀريو_دليلون ڍير_دليلون=UCNV_ڀريو_دليلون_INITIALIZER;

    UTRACE_داخلا_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    إذا(type<0 || UCNV_عدد_جو_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_دليل_ERROR;
        UTRACE_EXIT_حالت(U_ILLEGAL_دليل_ERROR);
        أرجع خالي_اشارو;
    }

    shared_معلومات = converter_معلومات[type];
    إذا(shared_معلومات == خالي_اشارو || shared_معلومات->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_دليل_ERROR;
        UTRACE_EXIT_حالت(U_ILLEGAL_دليل_ERROR);
        أرجع خالي_اشارو;
    }

    ڍير_دليلون.name = "";
    ڍير_دليلون.options = options;
    ڍير_دليلون.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_معلومات(
            myUConverter, const_cast<UConverter_Shared_معلومات*>(shared_معلومات),
            &ڍير_دليلون, err);

    UTRACE_EXIT_PTR_حالت(cnv, *err);
    أرجع cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ثابت حرف *packageName, ثابت حرف *converterName, UError_ڪوڊ * err)
{
    UConverter *myUConverter;
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات;
    UConverterNamePieces ڍير_Pieces;
    UConverter_ڀريو_دليلون ڍير_دليلون=UCNV_ڀريو_دليلون_INITIALIZER;

    UTRACE_داخلا_OC(UTRACE_UCNV_OPEN_PACKAGE);

    إذا(U_FAILURE(*err)) {
        UTRACE_EXIT_حالت(*err);
        أرجع خالي_اشارو;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ڍير_Pieces.cnvName[0] = 0;
    ڍير_Pieces.locale[0] = 0;
    ڍير_Pieces.options = 0;
    parseConverterOptions(converterName, &ڍير_Pieces, &ڍير_دليلون, err);
    إذا (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_حالت(*err);
        أرجع خالي_اشارو;
    }
    ڍير_دليلون.nestedLoads=1;
    ڍير_دليلون.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_معلومات = createConverterFromFile(&ڍير_دليلون, err);

    إذا (U_FAILURE(*err)) {
        UTRACE_EXIT_حالت(*err);
        أرجع خالي_اشارو;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_معلومات(خالي_اشارو, my_Shared_Converter_معلومات, &ڍير_دليلون, err);

    إذا (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_حالت(*err);
        أرجع خالي_اشارو;
    }

    UTRACE_EXIT_PTR_حالت(myUConverter, *err);
    أرجع myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_معلومات(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_معلومات* my_Shared_Converter_معلومات,
                                   UConverter_ڀريو_دليلون* p_دليلون,
                                   UError_ڪوڊ* err)
{
    UBool isCopyLocal;

    إذا(U_FAILURE(*err)) {
        ucnv_unload_Shared_معلومات_If_Ready(my_Shared_Converter_معلومات);
        أرجع myUConverter;
    }
    إذا(myUConverter == خالي_اشارو)
    {
        myUConverter = (UConverter *) uprv_malloc (حجم (UConverter));
        إذا(myUConverter == خالي_اشارو)
        {
            *err = U_يادگيري_ورهاست_ERROR;
            ucnv_unload_Shared_معلومات_If_Ready(my_Shared_Converter_معلومات);
            أرجع خالي_اشارو;
        }
        isCopyLocal = خطأ;
    } وإلا {
        isCopyLocal = سچ;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, حجم(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_معلومات = my_Shared_Converter_معلومات;
    myUConverter->options = p_دليلون->options;
    إذا(!p_دليلون->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_منزل_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->منزل_Unicode_حالت = my_Shared_Converter_معلومات->منزل_Unicode_حالت;
        myUConverter->max_بائيٽون_في_UChar = my_Shared_Converter_معلومات->static_معلومات->max_بائيٽون_في_Char;
        myUConverter->subChar1 = my_Shared_Converter_معلومات->static_معلومات->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_معلومات->static_معلومات->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_معلومات->static_معلومات->subChar, myUConverter->subCharLen);
        myUConverter->منزل_UCallback_Reason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    إذا(my_Shared_Converter_معلومات->impl->open != خالي_اشارو) {
        my_Shared_Converter_معلومات->impl->open(myUConverter, p_دليلون, err);
        إذا(U_FAILURE(*err) && !p_دليلون->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            أرجع خالي_اشارو;
        }
    }

    أرجع myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_معلومات *my_Shared_معلومات = خالي_اشارو;
    int32_t pos;
    int32_t جدول_Deleted_انگ = 0;
    ثابت UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, بچيل;

    UTRACE_داخلا_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    إذا (SHARED_معلومات_HASHTABLE == خالي_اشارو) {
        UTRACE_EXIT_قدر((int32_t)0);
        أرجع 0;
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
    umtx_تالو(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    افعل {
        بچيل = 0;
        pos = UHASH_پهريون;
        طالما ((e = uhash_ايندڙ_Element (SHARED_معلومات_HASHTABLE, &pos)) != خالي_اشارو)
        {
            my_Shared_معلومات = (UConverter_Shared_معلومات *) e->value.اشارو;
            /*deletes only if reference counter == 0 */
            إذا (my_Shared_معلومات->referenceCounter == 0)
            {
                جدول_Deleted_انگ++;

                UCNV_DEBUG_LOG("del",my_Shared_معلومات->static_معلومات->name,my_Shared_معلومات);

                uhash_removeElement(SHARED_معلومات_HASHTABLE, e);
                my_Shared_معلومات->shared_معلومات_Cached = خطأ;
                ucnv_delete_Shared_Converter_معلومات (my_Shared_معلومات);
            } وإلا {
                ++بچيل;
            }
        }
    } طالما(++i == 1 && بچيل > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", بچيل);

    UTRACE_EXIT_قدر(جدول_Deleted_انگ);
    أرجع جدول_Deleted_انگ;
}

/* available converters list --------------------------------------------------- */

ساكن فراغ U_CALLCONV init_موجود_Converters_List(UError_ڪوڊ &err_ڪوڊ) {
    U_ASSERT(g_موجود_Converter_ڳڻپ == 0);
    U_ASSERT(g_موجود_Converters == خالي_اشارو);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_open_All_نالا(&err_ڪوڊ);
    int32_t all_Converter_ڳڻپ = uenum_ڳڻپ(allConvEnum, &err_ڪوڊ);
    إذا (U_FAILURE(err_ڪوڊ)) {
        أرجع;
    }

    /* We can't have more than "*converterTable" converters to open */
    g_موجود_Converters = static_cast<ثابت حرف**>(uprv_malloc(all_Converter_ڳڻپ * حجم(حرف*)));
    إذا (!g_موجود_Converters) {
        err_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UError_ڪوڊ local_حالت = U_ZERO_ERROR;
    UConverter عارضي_Converter;
    ucnv_close(ucnv_createConverter(&عارضي_Converter, خالي_اشارو, &local_حالت));

    g_موجود_Converter_ڳڻپ = 0;

    لكل (int32_t idx = 0; idx < all_Converter_ڳڻپ; idx++) {
        local_حالت = U_ZERO_ERROR;
        ثابت حرف *converterName = uenum_ايندڙ(allConvEnum, خالي_اشارو, &local_حالت);
        إذا (ucnv_canCreateConverter(converterName, &local_حالت)) {
            g_موجود_Converters[g_موجود_Converter_ڳڻپ++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


ساكن UBool have_موجود_Converter_List(UError_ڪوڊ *p_Error_ڪوڊ) {
    umtx_initOnce(g_موجود_Converters_Init_Once, &init_موجود_Converters_List, *p_Error_ڪوڊ);
    أرجع U_SUCCESS(*p_Error_ڪوڊ);
}

U_CFUNC uint16_t
ucnv_bld_ڳڻپ_موجود_Converters(UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (have_موجود_Converter_List(p_Error_ڪوڊ)) {
        أرجع g_موجود_Converter_ڳڻپ;
    }
    أرجع 0;
}

U_CFUNC ثابت حرف *
ucnv_bld_get_موجود_Converter(uint16_t n, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (have_موجود_Converter_List(p_Error_ڪوڊ)) {
        إذا (n < g_موجود_Converter_ڳڻپ) {
            أرجع g_موجود_Converters[n];
        }
        *p_Error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
    }
    أرجع خالي_اشارو;
}

/* default converter name --------------------------------------------------- */

#إذا !U_CHARSET_IS_UTF8
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
ساكن مضمن فراغ
اندروني_سيٽ_Name(ثابت حرف *name, UError_ڪوڊ *حالت) {
    UConverterNamePieces ڍير_Pieces;
    UConverter_ڀريو_دليلون ڍير_دليلون=UCNV_ڀريو_دليلون_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool شامل_آهي_Option = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != خالي_اشارو);
    ثابت UConverter_Shared_معلومات *algorithmic_Shared_معلومات;

    ڍير_دليلون.name = name;
    إذا(شامل_آهي_Option) {
        ڍير_Pieces.cnvName[0] = 0;
        ڍير_Pieces.locale[0] = 0;
        ڍير_Pieces.options = 0;
        parseConverterOptions(name, &ڍير_Pieces, &ڍير_دليلون, حالت);
        إذا(U_FAILURE(*حالت)) {
            أرجع;
        }
    }
    algorithmic_Shared_معلومات = getAlgorithmicTypeFromName(ڍير_دليلون.name);

    umtx_تالو(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_معلومات = algorithmic_Shared_معلومات;
    g_Default_Converter_شامل_آهي_Option = شامل_آهي_Option;
    uprv_memcpy(g_Default_Converter_Name_بفر, name, length);
    g_Default_Converter_Name_بفر[length]=0;

    /* gDefaultConverterName MUST be the last global var set by this function.  */
    /*    It is the variable checked in ucnv_getDefaultName() to see if initialization is required. */
    //    But there is nothing here preventing that from being reordered, either by the compiler
    //             or hardware. I'm adding the mutex to ucnv_getDefaultName for now. UMTX_CHECK is not enough.
    //             -- Andy
    gDefaultConverterName = g_Default_Converter_Name_بفر;

    ucnv_enableCleanup();

    umtx_unlock(&cnvCacheMutex);
}
#جيڪڏهن_پورو

/*
 * In order to be really thread-safe, the get function would have to take
 * a buffer parameter and copy the current string inside a mutex block.
 * This implementation only tries to be really thread-safe while
 * setting the name.
 * It assumes that setting a pointer is atomic.
 */

U_CAPI ثابت حرف*  U_EXPORT2
ucnv_getDefaultName() {
#إذا U_CHARSET_IS_UTF8
    أرجع "UTF-8";
#وإلا
    /* local variable to be thread-safe */
    ثابت حرف *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex تالو(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    إذا(name==خالي_اشارو) {
        UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
        UConverter *cnv = خالي_اشارو;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        إذا(name != خالي_اشارو) {
            cnv = ucnv_open(name, &error_ڪوڊ);
            إذا(U_SUCCESS(error_ڪوڊ) && cnv != خالي_اشارو) {
                name = ucnv_getName(cnv, &error_ڪوڊ);
            }
        }

        إذا(name == خالي_اشارو || name[0] == 0
            || U_FAILURE(error_ڪوڊ) || cnv == خالي_اشارو
            || uprv_strlen(name)>=حجم(g_Default_Converter_Name_بفر))
        {
            /* Panic time, let's use a fallback. */
#إذا (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#وإلاإذا U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_مٽا_سٽا_LFNL_OPTION_STRING;
#وإلا
            name = "ibm-37_P100-1995";
#جيڪڏهن_پورو
        }

        اندروني_سيٽ_Name(name, &error_ڪوڊ);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    أرجع name;
#جيڪڏهن_پورو
}

#إذا U_CHARSET_IS_UTF8
U_CAPI فراغ U_EXPORT2 ucnv_سيٽ_Default_Name(ثابت حرف *) {}
#وإلا
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI فراغ U_EXPORT2
ucnv_سيٽ_Default_Name(ثابت حرف *converterName) {
    إذا(converterName==خالي_اشارو) {
        /* reset to the default codepage */
        gDefaultConverterName=خالي_اشارو;
    } وإلا {
        UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
        UConverter *cnv = خالي_اشارو;
        ثابت حرف *name = خالي_اشارو;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &error_ڪوڊ);
        إذا(U_SUCCESS(error_ڪوڊ) && cnv != خالي_اشارو) {
            name = ucnv_getName(cnv, &error_ڪوڊ);
        }

        إذا(U_SUCCESS(error_ڪوڊ) && name!=خالي_اشارو) {
            اندروني_سيٽ_Name(name, &error_ڪوڊ);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#جيڪڏهن_پورو

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#إذا !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_مٽا_سٽا(ثابت UDataSwapper *ds,
          ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
          UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UDataInfo *pInfo;
    int32_t سرو_سائيز;

    ثابت uint8_t *اندريون_بائيٽون;
    uint8_t *ٻاهريون_بائيٽون;

    uint32_t وٿي, count, static_معلومات_سائيز;
    int32_t size;

    ثابت UConverter_Static_معلومات *اندريون_Static_معلومات;
    UConverter_Static_معلومات *ٻاهريون_Static_معلومات;

    ثابت _MBCSHeader *اندريون_MBCSHeader;
    _MBCSHeader *ٻاهريون_MBCSHeader;
    _MBCSHeader mbcs_سرو;
    uint32_t mbcs_سرو_ڊيگهائي;
    UBool noFromU=خطأ;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_اشاريو_ڊيگهائي;

    ثابت int32_t *اندريون_Ext_Indexes;
    int32_t ext_وٿي;

    /* udata_swapDataHeader checks the arguments */
    سرو_سائيز=udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->معلومات_Format[1]==0x6e &&
        pInfo->معلومات_Format[2]==0x76 &&
        pInfo->معلومات_Format[3]==0x74 &&
        pInfo->format_نسخو[0]==6 &&
        pInfo->format_نسخو[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخو[0], pInfo->format_نسخو[1]);
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اندريون_بائيٽون=(ثابت uint8_t *)اندريون_معلومات+سرو_سائيز;
    ٻاهريون_بائيٽون=(ٻاهريون_معلومات == خالي_اشارو) ? خالي_اشارو : (uint8_t *)ٻاهريون_معلومات+سرو_سائيز;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    اندريون_Static_معلومات=(ثابت UConverter_Static_معلومات *)اندريون_بائيٽون;
    ٻاهريون_Static_معلومات=(UConverter_Static_معلومات *)ٻاهريون_بائيٽون;

    إذا(length<0) {
        static_معلومات_سائيز=ds->readUInt32(اندريون_Static_معلومات->struct_سائيز);
    } وإلا {
        length-=سرو_سائيز;
        إذا( length<(int32_t)حجم(UConverter_Static_معلومات) ||
            (uint32_t)length<(static_معلومات_سائيز=ds->readUInt32(اندريون_Static_معلومات->struct_سائيز))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    إذا(length>=0) {
        /* swap the static data */
        إذا(اندريون_Static_معلومات!=ٻاهريون_Static_معلومات) {
            uprv_memcpy(ٻاهريون_Static_معلومات, اندريون_Static_معلومات, static_معلومات_سائيز);
        }

        ds->مٽا_سٽا_Array32(ds, &اندريون_Static_معلومات->struct_سائيز, 4,
                           &ٻاهريون_Static_معلومات->struct_سائيز, p_Error_ڪوڊ);
        ds->مٽا_سٽا_Array32(ds, &اندريون_Static_معلومات->codepage, 4,
                           &ٻاهريون_Static_معلومات->codepage, p_Error_ڪوڊ);

        ds->مٽا_سٽا_Inv_Chars(ds, اندريون_Static_معلومات->name, (int32_t)uprv_strlen(اندريون_Static_معلومات->name),
                            ٻاهريون_Static_معلومات->name, p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            أرجع 0;
        }
    }

    اندريون_بائيٽون+=static_معلومات_سائيز;
    إذا (ٻاهريون_بائيٽون != خالي_اشارو) ٻاهريون_بائيٽون+=static_معلومات_سائيز;
    إذا(length>=0) {
        length-=(int32_t)static_معلومات_سائيز;
    }

    /* check for supported conversionType values */
    إذا(اندريون_Static_معلومات->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        اندريون_MBCSHeader=(ثابت _MBCSHeader *)اندريون_بائيٽون;
        ٻاهريون_MBCSHeader=(_MBCSHeader *)ٻاهريون_بائيٽون;

        إذا(0<=length && length<(int32_t)حجم(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        إذا(اندريون_MBCSHeader->نسخو[0]==4 && اندريون_MBCSHeader->نسخو[1]>=1) {
            mbcs_سرو_ڊيگهائي=MBCS_سرو_V4_ڊيگهائي;
        } وإلا إذا(اندريون_MBCSHeader->نسخو[0]==5 && اندريون_MBCSHeader->نسخو[1]>=3 &&
                  ((mbcs_سرو.options=ds->readUInt32(اندريون_MBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_نقاب)==0
        ) {
            mbcs_سرو_ڊيگهائي=mbcs_سرو.options&MBCS_OPT_ڊيگهائي_نقاب;
            noFromU = (mbcs_سرو.options & MBCS_OPT_NO_FROM_U) != 0;
        } وإلا {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             اندريون_MBCSHeader->نسخو[0], اندريون_MBCSHeader->نسخو[1]);
            *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        uprv_memcpy(mbcs_سرو.نسخو, اندريون_MBCSHeader->نسخو, 4);
        mbcs_سرو.ڳڻپ_حالتون=         ds->readUInt32(اندريون_MBCSHeader->ڳڻپ_حالتون);
        mbcs_سرو.ڳڻپ_منزل_UFallbacks=   ds->readUInt32(اندريون_MBCSHeader->ڳڻپ_منزل_UFallbacks);
        mbcs_سرو.وٿي_منزل_UCode_Units=  ds->readUInt32(اندريون_MBCSHeader->وٿي_منزل_UCode_Units);
        mbcs_سرو.وٿي_From_UTable=    ds->readUInt32(اندريون_MBCSHeader->وٿي_From_UTable);
        mbcs_سرو.وٿي_From_UBytes=    ds->readUInt32(اندريون_MBCSHeader->وٿي_From_UBytes);
        mbcs_سرو.flags=               ds->readUInt32(اندريون_MBCSHeader->flags);
        mbcs_سرو.from_UBytes_ڊيگهائي=    ds->readUInt32(اندريون_MBCSHeader->from_UBytes_ڊيگهائي);
        /* mbcsHeader.options have been read above */

        ext_وٿي=(int32_t)(mbcs_سرو.flags>>8);
        outputType=(uint8_t)mbcs_سرو.flags;
        إذا(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        /* make sure that the output type is known */
        بدل(outputType) {
        حالة MBCS_OUTPUT_1:
        حالة MBCS_OUTPUT_2:
        حالة MBCS_OUTPUT_3:
        حالة MBCS_OUTPUT_4:
        حالة MBCS_OUTPUT_3_EUC:
        حالة MBCS_OUTPUT_4_EUC:
        حالة MBCS_OUTPUT_2_SISO:
        حالة MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            اكسر;
        افتراضي:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        /* calculate the length of the MBCS data */

        /*
         * utf8Friendly MBCS files (mbcsHeader.version 4.3)
         * contain an additional mbcsIndex table:
         *   uint16_t[(maxFastUChar+1)>>6];
         * where maxFastUChar=((mbcsHeader.version[2]<<8)|0xff).
         */
        maxFastUChar=0;
        mbcs_اشاريو_ڊيگهائي=0;
        إذا( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcs_سرو.نسخو[1]>=3 && (maxFastUChar=mbcs_سرو.نسخو[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_اشاريو_ڊيگهائي=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        إذا(ext_وٿي==0) {
            size=(int32_t)(mbcs_سرو.وٿي_From_UBytes+mbcs_اشاريو_ڊيگهائي);
            إذا(!noFromU) {
                size+=(int32_t)mbcs_سرو.from_UBytes_ڊيگهائي;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            اندريون_Ext_Indexes=خالي_اشارو;
        } وإلا {
            /* there is extension data after the base data, see ucnv_ext.h */
            إذا(length>=0 && length<(ext_وٿي+UCNV_EXT_INDEXES_MIN_ڊيگهائي*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            اندريون_Ext_Indexes=(ثابت int32_t *)(اندريون_بائيٽون+ext_وٿي);
            size=ext_وٿي+udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_سائيز]);
        }

        إذا(length>=0) {
            إذا(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            /* copy the data for inaccessible bytes */
            إذا(اندريون_بائيٽون!=ٻاهريون_بائيٽون) {
                uprv_memcpy(ٻاهريون_بائيٽون, اندريون_بائيٽون, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_سرو_ڊيگهائي*4;
            ds->مٽا_سٽا_Array32(ds, &اندريون_MBCSHeader->ڳڻپ_حالتون, count-4,
                               &ٻاهريون_MBCSHeader->ڳڻپ_حالتون, p_Error_ڪوڊ);

            إذا(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ثابت حرف *اندريون_بنياد_Name=(ثابت حرف *)اندريون_بائيٽون+count;
                حرف *ٻاهريون_بنياد_Name=(حرف *)ٻاهريون_بائيٽون+count;
                ds->مٽا_سٽا_Inv_Chars(ds, اندريون_بنياد_Name, (int32_t)uprv_strlen(اندريون_بنياد_Name),
                                    ٻاهريون_بنياد_Name, p_Error_ڪوڊ);
            } وإلا {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                وٿي=count;
                count=mbcs_سرو.ڳڻپ_حالتون*1024;
                ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+وٿي, (int32_t)count,
                                   ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* swap the toUFallbacks[] */
                وٿي+=count;
                count=mbcs_سرو.ڳڻپ_منزل_UFallbacks*8;
                ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+وٿي, (int32_t)count,
                                   ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* swap the unicodeCodeUnits[] */
                وٿي=mbcs_سرو.وٿي_منزل_UCode_Units;
                count=mbcs_سرو.وٿي_From_UTable-وٿي;
                ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, (int32_t)count,
                                   ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* offset to the stage 1 table, independent of the outputType */
                وٿي=mbcs_سرو.وٿي_From_UTable;

                إذا(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcs_سرو.وٿي_From_UBytes-وٿي)+mbcs_سرو.from_UBytes_ڊيگهائي;
                    ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, (int32_t)count,
                                       ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);
                } وإلا {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    إذا(اندريون_Static_معلومات->unicode_نقاب&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } وإلا {
                        count=0x40*2; /* only BMP */
                    }
                    ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, (int32_t)count,
                                       ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                    /* stage 2 table: uint32_t[] */
                    وٿي+=count;
                    count=mbcs_سرو.وٿي_From_UBytes-وٿي;
                    ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+وٿي, (int32_t)count,
                                       ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    وٿي=mbcs_سرو.وٿي_From_UBytes;
                    count= noFromU ? 0 : mbcs_سرو.from_UBytes_ڊيگهائي;
                    بدل(outputType) {
                    حالة MBCS_OUTPUT_2:
                    حالة MBCS_OUTPUT_3_EUC:
                    حالة MBCS_OUTPUT_2_SISO:
                        ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, (int32_t)count,
                                           ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);
                        اكسر;
                    حالة MBCS_OUTPUT_4:
                        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+وٿي, (int32_t)count,
                                           ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);
                        اكسر;
                    افتراضي:
                        /* just uint8_t[], nothing to swap */
                        اكسر;
                    }

                    إذا(mbcs_اشاريو_ڊيگهائي!=0) {
                        وٿي+=count;
                        count=mbcs_اشاريو_ڊيگهائي;
                        ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, (int32_t)count,
                                           ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);
                    }
                }
            }

            إذا(ext_وٿي!=0) {
                /* swap the extension data */
                اندريون_بائيٽون+=ext_وٿي;
                ٻاهريون_بائيٽون+=ext_وٿي;

                /* swap toUTable[] */
                وٿي=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_منزل_U_اشاريو]);
                length=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_منزل_U_ڊيگهائي]);
                ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+وٿي, length*4, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* swap toUUChars[] */
                وٿي=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_منزل_U_UCHARS_اشاريو]);
                length=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_منزل_U_UCHARS_ڊيگهائي]);
                ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, length*2, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* swap fromUTableUChars[] */
                وٿي=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_FROM_U_UCHARS_اشاريو]);
                length=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_FROM_U_ڊيگهائي]);
                ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, length*2, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* swap fromUTableValues[] */
                وٿي=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_FROM_U_قدرون_اشاريو]);
                /* same length as for fromUTableUChars[] */
                ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+وٿي, length*4, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                وٿي=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_اشاريو]);
                length=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_ڊيگهائي]);
                ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, length*2, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* swap fromUStage3[] */
                وٿي=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_اشاريو]);
                length=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_ڊيگهائي]);
                ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, length*2, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* swap fromUStage3b[] */
                وٿي=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_اشاريو]);
                length=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_ڊيگهائي]);
                ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+وٿي, length*4, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);

                /* swap indexes[] */
                length=udata_readInt32(ds, اندريون_Ext_Indexes[UCNV_EXT_INDEXES_ڊيگهائي]);
                ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون, length*4, ٻاهريون_بائيٽون, p_Error_ڪوڊ);
            }
        }
    } وإلا {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         اندريون_Static_معلومات->conversionType);
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    أرجع سرو_سائيز+(int32_t)static_معلومات_سائيز+size;
}

#جيڪڏهن_پورو /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#جيڪڏهن_پورو
