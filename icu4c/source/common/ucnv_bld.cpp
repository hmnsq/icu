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
#تعریف_کړه UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#وإلا
# define UCNV_DEBUG_LOG(x,y,z)
#د_که_پای

ساكن ثابت UConverter_Shared_معلومات * ثابت
converter_معلومات[UCNV_شمېره_د_SUPPORTED_CONVERTER_TYPES]={
    تش_اشاره, تش_اشاره,

#إذا UCONFIG_NO_LEGACY_CONVERSION
    تش_اشاره,
#وإلا
    &_MBCSData,
#د_که_پای

    &Latin1_معلومات,
    &UTF8_معلومات, &_UTF16BEData, &_UTF16LEData,
#إذا UCONFIG_ONLY_HTML_CONVERSION
    تش_اشاره, تش_اشاره,
#وإلا
    &_UTF32BEData, &_UTF32LEData,
#د_که_پای
    تش_اشاره,

#إذا UCONFIG_NO_LEGACY_CONVERSION
    تش_اشاره,
#وإلا
    &ISO2022_معلومات,
#د_که_پای

#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    تش_اشاره, تش_اشاره, تش_اشاره, تش_اشاره, تش_اشاره, تش_اشاره,
    تش_اشاره, تش_اشاره, تش_اشاره, تش_اشاره, تش_اشاره, تش_اشاره,
    تش_اشاره,
#وإلا
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#د_که_پای

#إذا UCONFIG_ONLY_HTML_CONVERSION
    تش_اشاره,
#وإلا
    &_SCSUData,
#د_که_پای


#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    تش_اشاره,
#وإلا
    &_ISCIIData,
#د_که_پای

    &_ASCIIData,
#إذا UCONFIG_ONLY_HTML_CONVERSION
    تش_اشاره, تش_اشاره, &UTF16_معلومات, تش_اشاره, تش_اشاره, تش_اشاره,
#وإلا
    &UTF7_معلومات, &Bocu1_معلومات, &UTF16_معلومات, &UTF32_معلومات, &CESU8_معلومات, &_IMAPData,
#د_که_پای

#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    تش_اشاره,
#وإلا
    &Compound_متن_معلومات
#د_که_پای
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
#د_که_پای
#إذا !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#د_که_پای
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#د_که_پای
#إذا !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#د_که_پای
#إذا !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#د_که_پای
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
#د_که_پای
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#د_که_پای
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
#د_که_پای
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
#د_که_پای
#د_که_پای
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#د_که_پای
  { "utf8", UCNV_UTF8 },
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_متن}
#د_که_پای
};


/*initializes some global variables */
ساكن UHashtable *SHARED_معلومات_HASHTABLE = تش_اشاره;
ساكن icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

ساكن ثابت حرف **g_شته_Converters = تش_اشاره;
ساكن uint16_t g_شته_Converter_شمېر = 0;
ساكن icu::UInitOnce g_شته_Converters_Init_Once {};

#إذا !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
ساكن حرف g_Default_Converter_Name_بفر[UCNV_MAX_CONVERTER_NAME_اوږدتیا + 1]; /* +1 for nullptr */
ساكن ثابت حرف *gDefaultConverterName = تش_اشاره;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
ساكن ثابت UConverter_Shared_معلومات *g_Default_Algorithmic_Shared_معلومات = تش_اشاره;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
ساكن UBool g_Default_Converter_لري_Option;

#د_که_پای  /* !U_CHARSET_IS_UTF8 */

ساكن ثابت حرف معلومات_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
ساكن فراغ
ucnv_flush_شته_Converter_Cache() {
    g_شته_Converter_شمېر = 0;
    إذا (g_شته_Converters) {
        uprv_free(const_cast<حرف**>(g_شته_Converters));
        g_شته_Converters = تش_اشاره;
    }
    g_شته_Converters_Init_Once.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
ساكن UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    إذا (SHARED_معلومات_HASHTABLE != تش_اشاره && uhash_شمېر(SHARED_معلومات_HASHTABLE) == 0) {
        uhash_close(SHARED_معلومات_HASHTABLE);
        SHARED_معلومات_HASHTABLE = تش_اشاره;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flush_شته_Converter_Cache();

#إذا !U_CHARSET_IS_UTF8
    gDefaultConverterName = تش_اشاره;
    g_Default_Converter_Name_بفر[0] = 0;
    g_Default_Converter_لري_Option = خطأ;
    g_Default_Algorithmic_Shared_معلومات = تش_اشاره;
#د_که_پای

    أرجع (SHARED_معلومات_HASHTABLE == تش_اشاره);
}

U_CAPI فراغ U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
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
        pInfo->format_نسخه[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
ساكن UConverter_Shared_معلومات*
ucnv_معلومات_un_Flatten_Clone(UConverter_بارول_دلیلونه *p_دلیلونه, UData_حافظه *p_معلومات, UError_کوډ *حالت)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ثابت uint8_t* raw = static_cast<ثابت uint8_t*>(udata_get_حافظه(p_معلومات));
    ثابت UConverter_Static_معلومات* سرچینه = reinterpret_cast<ثابت UConverter_Static_معلومات*>(raw);
    UConverter_Shared_معلومات *معلومات;
    UConverterType type = static_cast<UConverterType>(سرچینه->conversionType);

    إذا(U_FAILURE(*حالت))
        أرجع تش_اشاره;

    إذا (static_cast<uint16_t>(type) >= UCNV_شمېره_د_SUPPORTED_CONVERTER_TYPES ||
        converter_معلومات[type] == تش_اشاره ||
        !converter_معلومات[type]->isReferenceCounted ||
        converter_معلومات[type]->referenceCounter != 1 ||
        سرچینه->struct_اندازه != حجم(UConverter_Static_معلومات))
    {
        *حالت = U_INVALID_جدول_FORMAT;
        أرجع تش_اشاره;
    }

    معلومات = static_cast<UConverter_Shared_معلومات*>(uprv_malloc(حجم(UConverter_Shared_معلومات)));
    إذا(معلومات == تش_اشاره) {
        *حالت = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(معلومات, converter_معلومات[type], حجم(UConverter_Shared_معلومات));

    معلومات->static_معلومات = سرچینه;

    معلومات->shared_معلومات_Cached = خطأ;

    /* fill in fields from the loaded data */
    معلومات->معلومات_حافظه = (فراغ*)p_معلومات; /* for future use */

    إذا(معلومات->impl->load != تش_اشاره) {
        معلومات->impl->load(معلومات, p_دلیلونه, raw + سرچینه->struct_اندازه, حالت);
        إذا(U_FAILURE(*حالت)) {
            uprv_free(معلومات);
            أرجع تش_اشاره;
        }
    }
    أرجع معلومات;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
ساكن UConverter_Shared_معلومات *createConverterFromFile(UConverter_بارول_دلیلونه *p_دلیلونه, UError_کوډ * err)
{
    UData_حافظه *معلومات;
    UConverter_Shared_معلومات *shared_معلومات;

    UTRACE_ثبت_OC(UTRACE_UCNV_بارول);

    إذا (U_FAILURE (*err)) {
        UTRACE_EXIT_حالت(*err);
        أرجع تش_اشاره;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", p_دلیلونه->name, p_دلیلونه->pkg);

    معلومات = udata_openChoice(p_دلیلونه->pkg, معلومات_TYPE, p_دلیلونه->name, isCnvAcceptable, تش_اشاره, err);
    إذا(U_FAILURE(*err))
    {
        UTRACE_EXIT_حالت(*err);
        أرجع تش_اشاره;
    }

    shared_معلومات = ucnv_معلومات_un_Flatten_Clone(p_دلیلونه, معلومات, err);
    إذا(U_FAILURE(*err))
    {
        udata_close(معلومات);
        UTRACE_EXIT_حالت(*err);
        أرجع تش_اشاره;
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
    uint32_t mid, پیل, حد;
    uint32_t وروستی_Mid;
    صحيح پایله;
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_اوږدتیا];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    پیل = 0;
    حد = UPRV_LENGTHOF(cnvNameType);
    mid = حد;
    وروستی_Mid = UINT32_MAX;

    لكل (;;) {
        mid = (پیل + حد) / 2;
        إذا (وروستی_Mid == mid) {   /* Have we moved? */
            اكسر;  /* We haven't moved, and it wasn't found. */
        }
        وروستی_Mid = mid;
        پایله = uprv_strcmp(strippedName, cnvNameType[mid].name);

        إذا (پایله < 0) {
            حد = mid;
        } وإلا إذا (پایله > 0) {
            پیل = mid;
        } وإلا {
            أرجع converter_معلومات[cnvNameType[mid].type];
        }
    }

    أرجع تش_اشاره;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#تعریف_کړه UCNV_CACHE_بارول_ضریب 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
ساكن فراغ
ucnv_share_Converter_معلومات(UConverter_Shared_معلومات * معلومات)
{
    UError_کوډ err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    إذا (SHARED_معلومات_HASHTABLE == تش_اشاره)
    {
        SHARED_معلومات_HASHTABLE = uhash_open_اندازه(uhash_hashChars, uhash_compareChars, تش_اشاره,
                            ucnv_io_شمېر_معلوم_Converters(&err)*UCNV_CACHE_بارول_ضریب,
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
    معلومات->shared_معلومات_Cached = ریښتیا;

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
    إذا (SHARED_معلومات_HASHTABLE == تش_اشاره)
    {
        أرجع تش_اشاره;
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
    UTRACE_ثبت_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_معلومات->static_معلومات->name, dead_Shared_معلومات);

    إذا (dead_Shared_معلومات->referenceCounter > 0) {
        UTRACE_EXIT_ارزښت((int32_t)خطأ);
        أرجع خطأ;
    }

    إذا (dead_Shared_معلومات->impl->unload != تش_اشاره) {
        dead_Shared_معلومات->impl->unload(dead_Shared_معلومات);
    }

    إذا(dead_Shared_معلومات->معلومات_حافظه != تش_اشاره)
    {
        UData_حافظه *معلومات = (UData_حافظه*)dead_Shared_معلومات->معلومات_حافظه;
        udata_close(معلومات);
    }

    uprv_free(dead_Shared_معلومات);

    UTRACE_EXIT_ارزښت((int32_t)ریښتیا);
    أرجع ریښتیا;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_معلومات *
ucnv_بارول(UConverter_بارول_دلیلونه *p_دلیلونه, UError_کوډ *err) {
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات;

    إذا(err == تش_اشاره || U_FAILURE(*err)) {
        أرجع تش_اشاره;
    }

    إذا(p_دلیلونه->pkg != تش_اشاره && *p_دلیلونه->pkg != 0) {
        /* application-provided converters are not currently cached */
        أرجع createConverterFromFile(p_دلیلونه, err);
    }

    my_Shared_Converter_معلومات = ucnv_get_Shared_Converter_معلومات(p_دلیلونه->name);
    إذا (my_Shared_Converter_معلومات == تش_اشاره)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_معلومات = createConverterFromFile(p_دلیلونه, err);
        إذا (U_FAILURE (*err) || (my_Shared_Converter_معلومات == تش_اشاره))
        {
            أرجع تش_اشاره;
        }
        وإلا إذا (!p_دلیلونه->onlyTestIsLoadable)
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
    إذا(shared_معلومات != تش_اشاره) {
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
    إذا(shared_معلومات != تش_اشاره && shared_معلومات->isReferenceCounted) {
        umtx_قلف(&cnvCacheMutex);
        ucnv_unload(shared_معلومات);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC فراغ
ucnv_increment_Ref_شمېر(UConverter_Shared_معلومات *shared_معلومات)
{
    إذا(shared_معلومات != تش_اشاره && shared_معلومات->isReferenceCounted) {
        umtx_قلف(&cnvCacheMutex);
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
parseConverterOptions(ثابت حرف *دننه_Name,
                      UConverterNamePieces *pPieces,
                      UConverter_بارول_دلیلونه *p_دلیلونه,
                      UError_کوډ *err)
{
    حرف *cnvName = pPieces->cnvName;
    حرف c;
    int32_t len = 0;

    p_دلیلونه->name=دننه_Name;
    p_دلیلونه->locale=pPieces->locale;
    p_دلیلونه->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    طالما((c=*دننه_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        إذا (++len>=UCNV_MAX_CONVERTER_NAME_اوږدتیا) {
            *err = U_ILLEGAL_دلیل_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            أرجع;
        }
        *cnvName++=c;
        دننه_Name++;
    }
    *cnvName=0;
    p_دلیلونه->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    طالما((c=*دننه_Name)!=0) {
        إذا(c==UCNV_OPTION_SEP_CHAR) {
            ++دننه_Name;
        }

        /* inName is behind an option separator */
        إذا(uprv_strncmp(دننه_Name, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            حرف *dest=pPieces->locale;

            /* copy the locale option value */
            دننه_Name+=7;
            len=0;
            طالما((c=*دننه_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++دننه_Name;

                إذا(++len>=ULOC_FULLNAME_ظرفیت) {
                    *err=U_ILLEGAL_دلیل_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    أرجع;
                }

                *dest++=c;
            }
            *dest=0;
        } وإلا إذا(uprv_strncmp(دننه_Name, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            دننه_Name+=8;
            c=*دننه_Name;
            إذا(c==0) {
                p_دلیلونه->options=(pPieces->options&=~UCNV_OPTION_نسخه);
                أرجع;
            } وإلا إذا (static_cast<uint8_t>(c - '0') < 10) {
                p_دلیلونه->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_نسخه) | static_cast<uint32_t>(c - '0');
                ++دننه_Name;
            }
        } وإلا إذا(uprv_strncmp(دننه_Name, "swaplfnl", 8)==0) {
            دننه_Name+=8;
            p_دلیلونه->options=(pPieces->options|=UCNV_OPTION_بدلون_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } وإلا {
            /* ignore any other options until we define some */
            طالما(((c = *دننه_Name++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
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
ucnv_بارول_Shared_معلومات(ثابت حرف *converterName,
                    UConverterNamePieces *pPieces,
                    UConverter_بارول_دلیلونه *p_دلیلونه,
                    UError_کوډ * err) {
    UConverterNamePieces ډېری_Pieces;
    UConverter_بارول_دلیلونه ډېری_دلیلونه;
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات = تش_اشاره;
    UError_کوډ دننی_Error_کوډ = U_ZERO_ERROR;
    UBool mayContainOption = ریښتیا;
    UBool checkForAlgorithmic = ریښتیا;

    إذا (U_FAILURE (*err)) {
        أرجع تش_اشاره;
    }

    إذا(pPieces == تش_اشاره) {
        إذا(p_دلیلونه != تش_اشاره) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_دننی_پروګرام_ERROR;
            أرجع تش_اشاره;
        }
        pPieces = &ډېری_Pieces;
    }
    إذا(p_دلیلونه == تش_اشاره) {
        uprv_memset(&ډېری_دلیلونه, 0, حجم(ډېری_دلیلونه));
        ډېری_دلیلونه.size = (int32_t)حجم(ډېری_دلیلونه);
        p_دلیلونه = &ډېری_دلیلونه;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    p_دلیلونه->name = converterName;
    p_دلیلونه->locale = pPieces->locale;
    p_دلیلونه->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    إذا (converterName == تش_اشاره) {
#إذا U_CHARSET_IS_UTF8
        p_دلیلونه->name = "UTF-8";
        أرجع (UConverter_Shared_معلومات *)converter_معلومات[UCNV_UTF8];
#وإلا
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        p_دلیلونه->name = ucnv_getDefaultName();
        إذا (p_دلیلونه->name == تش_اشاره) {
            *err = U_MISSING_RESOURCE_ERROR;
            أرجع تش_اشاره;
        }
        my_Shared_Converter_معلومات = (UConverter_Shared_معلومات *)g_Default_Algorithmic_Shared_معلومات;
        checkForAlgorithmic = خطأ;
        mayContainOption = g_Default_Converter_لري_Option;
        /* the default converter name is already canonical */
#د_که_پای
    }
    وإلا إذا(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        p_دلیلونه->name = "UTF-8";
        أرجع (UConverter_Shared_معلومات *)converter_معلومات[UCNV_UTF8];
    }
    وإلا {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, p_دلیلونه, err);
        إذا (U_FAILURE(*err)) {
            /* Very bad name used. */
            أرجع تش_اشاره;
        }

        /* get the canonical converter name */
        p_دلیلونه->name = ucnv_io_getConverterName(p_دلیلونه->name, &mayContainOption, &دننی_Error_کوډ);
        إذا (U_FAILURE(دننی_Error_کوډ) || p_دلیلونه->name == تش_اشاره) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            p_دلیلونه->name = pPieces->cnvName;
        } وإلا إذا (دننی_Error_کوډ == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    إذا(mayContainOption && p_دلیلونه->name != pPieces->cnvName) {
        parseConverterOptions(p_دلیلونه->name, pPieces, p_دلیلونه, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    إذا (checkForAlgorithmic) {
        my_Shared_Converter_معلومات = (UConverter_Shared_معلومات *)getAlgorithmicTypeFromName(p_دلیلونه->name);
    }
    إذا (my_Shared_Converter_معلومات == تش_اشاره)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        p_دلیلونه->nestedLoads=1;
        p_دلیلونه->pkg=تش_اشاره;

        umtx_قلف(&cnvCacheMutex);
        my_Shared_Converter_معلومات = ucnv_بارول(p_دلیلونه, err);
        umtx_unlock(&cnvCacheMutex);
        إذا (U_FAILURE (*err) || (my_Shared_Converter_معلومات == تش_اشاره))
        {
            أرجع تش_اشاره;
        }
    }

    أرجع my_Shared_Converter_معلومات;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ثابت حرف* converterName,
                     UError_کوډ* err)
{
    UConverterNamePieces ډېری_Pieces;
    UConverter_بارول_دلیلونه ډېری_دلیلونه=UCNV_بارول_دلیلونه_INITIALIZER;
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات;

    UTRACE_ثبت_OC(UTRACE_UCNV_OPEN);

    إذا(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_معلومات = ucnv_بارول_Shared_معلومات(converterName, &ډېری_Pieces, &ډېری_دلیلونه, err);

        myUConverter = ucnv_create_Converter_From_Shared_معلومات(
            myUConverter, my_Shared_Converter_معلومات,
            &ډېری_دلیلونه,
            err);

        إذا(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_حالت(myUConverter, *err);
            أرجع myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_حالت(*err);
    أرجع تش_اشاره;
}

U_CFUNC UBool
ucnv_canCreateConverter(ثابت حرف *converterName, UError_کوډ *err) {
    UConverter myUConverter;
    UConverterNamePieces ډېری_Pieces;
    UConverter_بارول_دلیلونه ډېری_دلیلونه=UCNV_بارول_دلیلونه_INITIALIZER;
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات;

    UTRACE_ثبت_OC(UTRACE_UCNV_OPEN);

    إذا(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ډېری_دلیلونه.onlyTestIsLoadable=ریښتیا;
        my_Shared_Converter_معلومات = ucnv_بارول_Shared_معلومات(converterName, &ډېری_Pieces, &ډېری_دلیلونه, err);
        ucnv_create_Converter_From_Shared_معلومات(
            &myUConverter, my_Shared_Converter_معلومات,
            &ډېری_دلیلونه,
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
                                UError_کوډ* err) {
    UConverter *cnv;
    ثابت UConverter_Shared_معلومات *shared_معلومات;
    UConverter_بارول_دلیلونه ډېری_دلیلونه=UCNV_بارول_دلیلونه_INITIALIZER;

    UTRACE_ثبت_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    إذا(type<0 || UCNV_شمېره_د_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_دلیل_ERROR;
        UTRACE_EXIT_حالت(U_ILLEGAL_دلیل_ERROR);
        أرجع تش_اشاره;
    }

    shared_معلومات = converter_معلومات[type];
    إذا(shared_معلومات == تش_اشاره || shared_معلومات->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_دلیل_ERROR;
        UTRACE_EXIT_حالت(U_ILLEGAL_دلیل_ERROR);
        أرجع تش_اشاره;
    }

    ډېری_دلیلونه.name = "";
    ډېری_دلیلونه.options = options;
    ډېری_دلیلونه.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_معلومات(
            myUConverter, const_cast<UConverter_Shared_معلومات*>(shared_معلومات),
            &ډېری_دلیلونه, err);

    UTRACE_EXIT_PTR_حالت(cnv, *err);
    أرجع cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ثابت حرف *packageName, ثابت حرف *converterName, UError_کوډ * err)
{
    UConverter *myUConverter;
    UConverter_Shared_معلومات *my_Shared_Converter_معلومات;
    UConverterNamePieces ډېری_Pieces;
    UConverter_بارول_دلیلونه ډېری_دلیلونه=UCNV_بارول_دلیلونه_INITIALIZER;

    UTRACE_ثبت_OC(UTRACE_UCNV_OPEN_PACKAGE);

    إذا(U_FAILURE(*err)) {
        UTRACE_EXIT_حالت(*err);
        أرجع تش_اشاره;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ډېری_Pieces.cnvName[0] = 0;
    ډېری_Pieces.locale[0] = 0;
    ډېری_Pieces.options = 0;
    parseConverterOptions(converterName, &ډېری_Pieces, &ډېری_دلیلونه, err);
    إذا (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_حالت(*err);
        أرجع تش_اشاره;
    }
    ډېری_دلیلونه.nestedLoads=1;
    ډېری_دلیلونه.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_معلومات = createConverterFromFile(&ډېری_دلیلونه, err);

    إذا (U_FAILURE(*err)) {
        UTRACE_EXIT_حالت(*err);
        أرجع تش_اشاره;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_معلومات(تش_اشاره, my_Shared_Converter_معلومات, &ډېری_دلیلونه, err);

    إذا (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_حالت(*err);
        أرجع تش_اشاره;
    }

    UTRACE_EXIT_PTR_حالت(myUConverter, *err);
    أرجع myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_معلومات(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_معلومات* my_Shared_Converter_معلومات,
                                   UConverter_بارول_دلیلونه* p_دلیلونه,
                                   UError_کوډ* err)
{
    UBool isCopyLocal;

    إذا(U_FAILURE(*err)) {
        ucnv_unload_Shared_معلومات_If_Ready(my_Shared_Converter_معلومات);
        أرجع myUConverter;
    }
    إذا(myUConverter == تش_اشاره)
    {
        myUConverter = (UConverter *) uprv_malloc (حجم (UConverter));
        إذا(myUConverter == تش_اشاره)
        {
            *err = U_حافظه_ویش_ERROR;
            ucnv_unload_Shared_معلومات_If_Ready(my_Shared_Converter_معلومات);
            أرجع تش_اشاره;
        }
        isCopyLocal = خطأ;
    } وإلا {
        isCopyLocal = ریښتیا;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, حجم(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_معلومات = my_Shared_Converter_معلومات;
    myUConverter->options = p_دلیلونه->options;
    إذا(!p_دلیلونه->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_منزل_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->منزل_Unicode_حالت = my_Shared_Converter_معلومات->منزل_Unicode_حالت;
        myUConverter->max_بایټونه_فی_UChar = my_Shared_Converter_معلومات->static_معلومات->max_بایټونه_فی_Char;
        myUConverter->subChar1 = my_Shared_Converter_معلومات->static_معلومات->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_معلومات->static_معلومات->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_معلومات->static_معلومات->subChar, myUConverter->subCharLen);
        myUConverter->منزل_UCallback_Reason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    إذا(my_Shared_Converter_معلومات->impl->open != تش_اشاره) {
        my_Shared_Converter_معلومات->impl->open(myUConverter, p_دلیلونه, err);
        إذا(U_FAILURE(*err) && !p_دلیلونه->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            أرجع تش_اشاره;
        }
    }

    أرجع myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_معلومات *my_Shared_معلومات = تش_اشاره;
    int32_t pos;
    int32_t جدول_Deleted_عدد = 0;
    ثابت UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, پاتې_شوی;

    UTRACE_ثبت_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    إذا (SHARED_معلومات_HASHTABLE == تش_اشاره) {
        UTRACE_EXIT_ارزښت((int32_t)0);
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
    umtx_قلف(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    افعل {
        پاتې_شوی = 0;
        pos = UHASH_لومړی;
        طالما ((e = uhash_بل_Element (SHARED_معلومات_HASHTABLE, &pos)) != تش_اشاره)
        {
            my_Shared_معلومات = (UConverter_Shared_معلومات *) e->value.اشاره_ګر;
            /*deletes only if reference counter == 0 */
            إذا (my_Shared_معلومات->referenceCounter == 0)
            {
                جدول_Deleted_عدد++;

                UCNV_DEBUG_LOG("del",my_Shared_معلومات->static_معلومات->name,my_Shared_معلومات);

                uhash_removeElement(SHARED_معلومات_HASHTABLE, e);
                my_Shared_معلومات->shared_معلومات_Cached = خطأ;
                ucnv_delete_Shared_Converter_معلومات (my_Shared_معلومات);
            } وإلا {
                ++پاتې_شوی;
            }
        }
    } طالما(++i == 1 && پاتې_شوی > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", پاتې_شوی);

    UTRACE_EXIT_ارزښت(جدول_Deleted_عدد);
    أرجع جدول_Deleted_عدد;
}

/* available converters list --------------------------------------------------- */

ساكن فراغ U_CALLCONV init_شته_Converters_List(UError_کوډ &err_کوډ) {
    U_ASSERT(g_شته_Converter_شمېر == 0);
    U_ASSERT(g_شته_Converters == تش_اشاره);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_open_All_نومونه(&err_کوډ);
    int32_t all_Converter_شمېر = uenum_شمېر(allConvEnum, &err_کوډ);
    إذا (U_FAILURE(err_کوډ)) {
        أرجع;
    }

    /* We can't have more than "*converterTable" converters to open */
    g_شته_Converters = static_cast<ثابت حرف**>(uprv_malloc(all_Converter_شمېر * حجم(حرف*)));
    إذا (!g_شته_Converters) {
        err_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UError_کوډ local_حالت = U_ZERO_ERROR;
    UConverter لنډمهاله_Converter;
    ucnv_close(ucnv_createConverter(&لنډمهاله_Converter, تش_اشاره, &local_حالت));

    g_شته_Converter_شمېر = 0;

    لكل (int32_t idx = 0; idx < all_Converter_شمېر; idx++) {
        local_حالت = U_ZERO_ERROR;
        ثابت حرف *converterName = uenum_بل(allConvEnum, تش_اشاره, &local_حالت);
        إذا (ucnv_canCreateConverter(converterName, &local_حالت)) {
            g_شته_Converters[g_شته_Converter_شمېر++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


ساكن UBool have_شته_Converter_List(UError_کوډ *p_Error_کوډ) {
    umtx_initOnce(g_شته_Converters_Init_Once, &init_شته_Converters_List, *p_Error_کوډ);
    أرجع U_SUCCESS(*p_Error_کوډ);
}

U_CFUNC uint16_t
ucnv_bld_شمېر_شته_Converters(UError_کوډ *p_Error_کوډ) {
    إذا (have_شته_Converter_List(p_Error_کوډ)) {
        أرجع g_شته_Converter_شمېر;
    }
    أرجع 0;
}

U_CFUNC ثابت حرف *
ucnv_bld_get_شته_Converter(uint16_t n, UError_کوډ *p_Error_کوډ) {
    إذا (have_شته_Converter_List(p_Error_کوډ)) {
        إذا (n < g_شته_Converter_شمېر) {
            أرجع g_شته_Converters[n];
        }
        *p_Error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
    }
    أرجع تش_اشاره;
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
دننی_سېټ_Name(ثابت حرف *name, UError_کوډ *حالت) {
    UConverterNamePieces ډېری_Pieces;
    UConverter_بارول_دلیلونه ډېری_دلیلونه=UCNV_بارول_دلیلونه_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool لري_Option = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != تش_اشاره);
    ثابت UConverter_Shared_معلومات *algorithmic_Shared_معلومات;

    ډېری_دلیلونه.name = name;
    إذا(لري_Option) {
        ډېری_Pieces.cnvName[0] = 0;
        ډېری_Pieces.locale[0] = 0;
        ډېری_Pieces.options = 0;
        parseConverterOptions(name, &ډېری_Pieces, &ډېری_دلیلونه, حالت);
        إذا(U_FAILURE(*حالت)) {
            أرجع;
        }
    }
    algorithmic_Shared_معلومات = getAlgorithmicTypeFromName(ډېری_دلیلونه.name);

    umtx_قلف(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_معلومات = algorithmic_Shared_معلومات;
    g_Default_Converter_لري_Option = لري_Option;
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
#د_که_پای

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
        icu::Mutex قلف(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    إذا(name==تش_اشاره) {
        UError_کوډ error_کوډ = U_ZERO_ERROR;
        UConverter *cnv = تش_اشاره;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        إذا(name != تش_اشاره) {
            cnv = ucnv_open(name, &error_کوډ);
            إذا(U_SUCCESS(error_کوډ) && cnv != تش_اشاره) {
                name = ucnv_getName(cnv, &error_کوډ);
            }
        }

        إذا(name == تش_اشاره || name[0] == 0
            || U_FAILURE(error_کوډ) || cnv == تش_اشاره
            || uprv_strlen(name)>=حجم(g_Default_Converter_Name_بفر))
        {
            /* Panic time, let's use a fallback. */
#إذا (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#وإلاإذا U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_بدلون_LFNL_OPTION_STRING;
#وإلا
            name = "ibm-37_P100-1995";
#د_که_پای
        }

        دننی_سېټ_Name(name, &error_کوډ);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    أرجع name;
#د_که_پای
}

#إذا U_CHARSET_IS_UTF8
U_CAPI فراغ U_EXPORT2 ucnv_سېټ_Default_Name(ثابت حرف *) {}
#وإلا
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI فراغ U_EXPORT2
ucnv_سېټ_Default_Name(ثابت حرف *converterName) {
    إذا(converterName==تش_اشاره) {
        /* reset to the default codepage */
        gDefaultConverterName=تش_اشاره;
    } وإلا {
        UError_کوډ error_کوډ = U_ZERO_ERROR;
        UConverter *cnv = تش_اشاره;
        ثابت حرف *name = تش_اشاره;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &error_کوډ);
        إذا(U_SUCCESS(error_کوډ) && cnv != تش_اشاره) {
            name = ucnv_getName(cnv, &error_کوډ);
        }

        إذا(U_SUCCESS(error_کوډ) && name!=تش_اشاره) {
            دننی_سېټ_Name(name, &error_کوډ);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#د_که_پای

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#إذا !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_بدلون(ثابت UDataSwapper *ds,
          ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
          UError_کوډ *p_Error_کوډ) {
    ثابت UDataInfo *pInfo;
    int32_t سرلیک_اندازه;

    ثابت uint8_t *دننه_بایټونه;
    uint8_t *بهر_بایټونه;

    uint32_t دننه_واټن, count, static_معلومات_اندازه;
    int32_t size;

    ثابت UConverter_Static_معلومات *دننه_Static_معلومات;
    UConverter_Static_معلومات *بهر_Static_معلومات;

    ثابت _MBCSHeader *دننه_MBCSHeader;
    _MBCSHeader *بهر_MBCSHeader;
    _MBCSHeader mbcs_سرلیک;
    uint32_t mbcs_سرلیک_اوږدتیا;
    UBool noFromU=خطأ;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_شاخص_اوږدتیا;

    ثابت int32_t *دننه_Ext_Indexes;
    int32_t ext_دننه_واټن;

    /* udata_swapDataHeader checks the arguments */
    سرلیک_اندازه=udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->معلومات_Format[1]==0x6e &&
        pInfo->معلومات_Format[2]==0x76 &&
        pInfo->معلومات_Format[3]==0x74 &&
        pInfo->format_نسخه[0]==6 &&
        pInfo->format_نسخه[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخه[0], pInfo->format_نسخه[1]);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    دننه_بایټونه=(ثابت uint8_t *)دننه_معلومات+سرلیک_اندازه;
    بهر_بایټونه=(بهر_معلومات == تش_اشاره) ? تش_اشاره : (uint8_t *)بهر_معلومات+سرلیک_اندازه;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    دننه_Static_معلومات=(ثابت UConverter_Static_معلومات *)دننه_بایټونه;
    بهر_Static_معلومات=(UConverter_Static_معلومات *)بهر_بایټونه;

    إذا(length<0) {
        static_معلومات_اندازه=ds->readUInt32(دننه_Static_معلومات->struct_اندازه);
    } وإلا {
        length-=سرلیک_اندازه;
        إذا( length<(int32_t)حجم(UConverter_Static_معلومات) ||
            (uint32_t)length<(static_معلومات_اندازه=ds->readUInt32(دننه_Static_معلومات->struct_اندازه))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    إذا(length>=0) {
        /* swap the static data */
        إذا(دننه_Static_معلومات!=بهر_Static_معلومات) {
            uprv_memcpy(بهر_Static_معلومات, دننه_Static_معلومات, static_معلومات_اندازه);
        }

        ds->بدلون_Array32(ds, &دننه_Static_معلومات->struct_اندازه, 4,
                           &بهر_Static_معلومات->struct_اندازه, p_Error_کوډ);
        ds->بدلون_Array32(ds, &دننه_Static_معلومات->codepage, 4,
                           &بهر_Static_معلومات->codepage, p_Error_کوډ);

        ds->بدلون_Inv_Chars(ds, دننه_Static_معلومات->name, (int32_t)uprv_strlen(دننه_Static_معلومات->name),
                            بهر_Static_معلومات->name, p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            أرجع 0;
        }
    }

    دننه_بایټونه+=static_معلومات_اندازه;
    إذا (بهر_بایټونه != تش_اشاره) بهر_بایټونه+=static_معلومات_اندازه;
    إذا(length>=0) {
        length-=(int32_t)static_معلومات_اندازه;
    }

    /* check for supported conversionType values */
    إذا(دننه_Static_معلومات->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        دننه_MBCSHeader=(ثابت _MBCSHeader *)دننه_بایټونه;
        بهر_MBCSHeader=(_MBCSHeader *)بهر_بایټونه;

        إذا(0<=length && length<(int32_t)حجم(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        إذا(دننه_MBCSHeader->نسخه[0]==4 && دننه_MBCSHeader->نسخه[1]>=1) {
            mbcs_سرلیک_اوږدتیا=MBCS_سرلیک_V4_اوږدتیا;
        } وإلا إذا(دننه_MBCSHeader->نسخه[0]==5 && دننه_MBCSHeader->نسخه[1]>=3 &&
                  ((mbcs_سرلیک.options=ds->readUInt32(دننه_MBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_ماسک)==0
        ) {
            mbcs_سرلیک_اوږدتیا=mbcs_سرلیک.options&MBCS_OPT_اوږدتیا_ماسک;
            noFromU = (mbcs_سرلیک.options & MBCS_OPT_NO_FROM_U) != 0;
        } وإلا {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             دننه_MBCSHeader->نسخه[0], دننه_MBCSHeader->نسخه[1]);
            *p_Error_کوډ=U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        uprv_memcpy(mbcs_سرلیک.نسخه, دننه_MBCSHeader->نسخه, 4);
        mbcs_سرلیک.شمېر_حالتونه=         ds->readUInt32(دننه_MBCSHeader->شمېر_حالتونه);
        mbcs_سرلیک.شمېر_منزل_UFallbacks=   ds->readUInt32(دننه_MBCSHeader->شمېر_منزل_UFallbacks);
        mbcs_سرلیک.دننه_واټن_منزل_UCode_Units=  ds->readUInt32(دننه_MBCSHeader->دننه_واټن_منزل_UCode_Units);
        mbcs_سرلیک.دننه_واټن_From_UTable=    ds->readUInt32(دننه_MBCSHeader->دننه_واټن_From_UTable);
        mbcs_سرلیک.دننه_واټن_From_UBytes=    ds->readUInt32(دننه_MBCSHeader->دننه_واټن_From_UBytes);
        mbcs_سرلیک.flags=               ds->readUInt32(دننه_MBCSHeader->flags);
        mbcs_سرلیک.from_UBytes_اوږدتیا=    ds->readUInt32(دننه_MBCSHeader->from_UBytes_اوږدتیا);
        /* mbcsHeader.options have been read above */

        ext_دننه_واټن=(int32_t)(mbcs_سرلیک.flags>>8);
        outputType=(uint8_t)mbcs_سرلیک.flags;
        إذا(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *p_Error_کوډ=U_UNSUPPORTED_ERROR;
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
            *p_Error_کوډ=U_UNSUPPORTED_ERROR;
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
        mbcs_شاخص_اوږدتیا=0;
        إذا( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcs_سرلیک.نسخه[1]>=3 && (maxFastUChar=mbcs_سرلیک.نسخه[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_شاخص_اوږدتیا=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        إذا(ext_دننه_واټن==0) {
            size=(int32_t)(mbcs_سرلیک.دننه_واټن_From_UBytes+mbcs_شاخص_اوږدتیا);
            إذا(!noFromU) {
                size+=(int32_t)mbcs_سرلیک.from_UBytes_اوږدتیا;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            دننه_Ext_Indexes=تش_اشاره;
        } وإلا {
            /* there is extension data after the base data, see ucnv_ext.h */
            إذا(length>=0 && length<(ext_دننه_واټن+UCNV_EXT_INDEXES_MIN_اوږدتیا*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            دننه_Ext_Indexes=(ثابت int32_t *)(دننه_بایټونه+ext_دننه_واټن);
            size=ext_دننه_واټن+udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_اندازه]);
        }

        إذا(length>=0) {
            إذا(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            /* copy the data for inaccessible bytes */
            إذا(دننه_بایټونه!=بهر_بایټونه) {
                uprv_memcpy(بهر_بایټونه, دننه_بایټونه, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_سرلیک_اوږدتیا*4;
            ds->بدلون_Array32(ds, &دننه_MBCSHeader->شمېر_حالتونه, count-4,
                               &بهر_MBCSHeader->شمېر_حالتونه, p_Error_کوډ);

            إذا(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ثابت حرف *دننه_بنسټ_Name=(ثابت حرف *)دننه_بایټونه+count;
                حرف *بهر_بنسټ_Name=(حرف *)بهر_بایټونه+count;
                ds->بدلون_Inv_Chars(ds, دننه_بنسټ_Name, (int32_t)uprv_strlen(دننه_بنسټ_Name),
                                    بهر_بنسټ_Name, p_Error_کوډ);
            } وإلا {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                دننه_واټن=count;
                count=mbcs_سرلیک.شمېر_حالتونه*1024;
                ds->بدلون_Array32(ds, دننه_بایټونه+دننه_واټن, (int32_t)count,
                                   بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* swap the toUFallbacks[] */
                دننه_واټن+=count;
                count=mbcs_سرلیک.شمېر_منزل_UFallbacks*8;
                ds->بدلون_Array32(ds, دننه_بایټونه+دننه_واټن, (int32_t)count,
                                   بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* swap the unicodeCodeUnits[] */
                دننه_واټن=mbcs_سرلیک.دننه_واټن_منزل_UCode_Units;
                count=mbcs_سرلیک.دننه_واټن_From_UTable-دننه_واټن;
                ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, (int32_t)count,
                                   بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* offset to the stage 1 table, independent of the outputType */
                دننه_واټن=mbcs_سرلیک.دننه_واټن_From_UTable;

                إذا(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcs_سرلیک.دننه_واټن_From_UBytes-دننه_واټن)+mbcs_سرلیک.from_UBytes_اوږدتیا;
                    ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, (int32_t)count,
                                       بهر_بایټونه+دننه_واټن, p_Error_کوډ);
                } وإلا {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    إذا(دننه_Static_معلومات->unicode_ماسک&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } وإلا {
                        count=0x40*2; /* only BMP */
                    }
                    ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, (int32_t)count,
                                       بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                    /* stage 2 table: uint32_t[] */
                    دننه_واټن+=count;
                    count=mbcs_سرلیک.دننه_واټن_From_UBytes-دننه_واټن;
                    ds->بدلون_Array32(ds, دننه_بایټونه+دننه_واټن, (int32_t)count,
                                       بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    دننه_واټن=mbcs_سرلیک.دننه_واټن_From_UBytes;
                    count= noFromU ? 0 : mbcs_سرلیک.from_UBytes_اوږدتیا;
                    بدل(outputType) {
                    حالة MBCS_OUTPUT_2:
                    حالة MBCS_OUTPUT_3_EUC:
                    حالة MBCS_OUTPUT_2_SISO:
                        ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, (int32_t)count,
                                           بهر_بایټونه+دننه_واټن, p_Error_کوډ);
                        اكسر;
                    حالة MBCS_OUTPUT_4:
                        ds->بدلون_Array32(ds, دننه_بایټونه+دننه_واټن, (int32_t)count,
                                           بهر_بایټونه+دننه_واټن, p_Error_کوډ);
                        اكسر;
                    افتراضي:
                        /* just uint8_t[], nothing to swap */
                        اكسر;
                    }

                    إذا(mbcs_شاخص_اوږدتیا!=0) {
                        دننه_واټن+=count;
                        count=mbcs_شاخص_اوږدتیا;
                        ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, (int32_t)count,
                                           بهر_بایټونه+دننه_واټن, p_Error_کوډ);
                    }
                }
            }

            إذا(ext_دننه_واټن!=0) {
                /* swap the extension data */
                دننه_بایټونه+=ext_دننه_واټن;
                بهر_بایټونه+=ext_دننه_واټن;

                /* swap toUTable[] */
                دننه_واټن=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_منزل_U_شاخص]);
                length=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_منزل_U_اوږدتیا]);
                ds->بدلون_Array32(ds, دننه_بایټونه+دننه_واټن, length*4, بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* swap toUUChars[] */
                دننه_واټن=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_منزل_U_UCHARS_شاخص]);
                length=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_منزل_U_UCHARS_اوږدتیا]);
                ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, length*2, بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* swap fromUTableUChars[] */
                دننه_واټن=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_FROM_U_UCHARS_شاخص]);
                length=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_FROM_U_اوږدتیا]);
                ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, length*2, بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* swap fromUTableValues[] */
                دننه_واټن=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_FROM_U_ارزښتونه_شاخص]);
                /* same length as for fromUTableUChars[] */
                ds->بدلون_Array32(ds, دننه_بایټونه+دننه_واټن, length*4, بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                دننه_واټن=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_شاخص]);
                length=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_اوږدتیا]);
                ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, length*2, بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* swap fromUStage3[] */
                دننه_واټن=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_شاخص]);
                length=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_اوږدتیا]);
                ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, length*2, بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* swap fromUStage3b[] */
                دننه_واټن=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_شاخص]);
                length=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_اوږدتیا]);
                ds->بدلون_Array32(ds, دننه_بایټونه+دننه_واټن, length*4, بهر_بایټونه+دننه_واټن, p_Error_کوډ);

                /* swap indexes[] */
                length=udata_readInt32(ds, دننه_Ext_Indexes[UCNV_EXT_INDEXES_اوږدتیا]);
                ds->بدلون_Array32(ds, دننه_بایټونه, length*4, بهر_بایټونه, p_Error_کوډ);
            }
        }
    } وإلا {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         دننه_Static_معلومات->conversionType);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    أرجع سرلیک_اندازه+(int32_t)static_معلومات_اندازه+size;
}

#د_که_پای /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#د_که_پای
