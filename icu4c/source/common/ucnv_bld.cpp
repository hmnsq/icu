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
#تعریف_کن UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#وإلا
# define UCNV_DEBUG_LOG(x,y,z)
#پایان_اگر

ساكن ثابت UConverter_Shared_داده * ثابت
converter_داده[UCNV_عدد_از_SUPPORTED_CONVERTER_TYPES]={
    اشاره_تهی, اشاره_تهی,

#إذا UCONFIG_NO_LEGACY_CONVERSION
    اشاره_تهی,
#وإلا
    &_MBCSData,
#پایان_اگر

    &Latin1_داده,
    &UTF8_داده, &_UTF16BEData, &_UTF16LEData,
#إذا UCONFIG_ONLY_HTML_CONVERSION
    اشاره_تهی, اشاره_تهی,
#وإلا
    &_UTF32BEData, &_UTF32LEData,
#پایان_اگر
    اشاره_تهی,

#إذا UCONFIG_NO_LEGACY_CONVERSION
    اشاره_تهی,
#وإلا
    &ISO2022_داده,
#پایان_اگر

#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی,
    اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی,
    اشاره_تهی,
#وإلا
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#پایان_اگر

#إذا UCONFIG_ONLY_HTML_CONVERSION
    اشاره_تهی,
#وإلا
    &_SCSUData,
#پایان_اگر


#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    اشاره_تهی,
#وإلا
    &_ISCIIData,
#پایان_اگر

    &_ASCIIData,
#إذا UCONFIG_ONLY_HTML_CONVERSION
    اشاره_تهی, اشاره_تهی, &UTF16_داده, اشاره_تهی, اشاره_تهی, اشاره_تهی,
#وإلا
    &UTF7_داده, &Bocu1_داده, &UTF16_داده, &UTF32_داده, &CESU8_داده, &_IMAPData,
#پایان_اگر

#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    اشاره_تهی,
#وإلا
    &Compound_متن_داده
#پایان_اگر
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
#پایان_اگر
#إذا !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#پایان_اگر
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#پایان_اگر
#إذا !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#پایان_اگر
#إذا !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#پایان_اگر
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
#پایان_اگر
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#پایان_اگر
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
#پایان_اگر
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
#پایان_اگر
#پایان_اگر
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#پایان_اگر
  { "utf8", UCNV_UTF8 },
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_متن}
#پایان_اگر
};


/*initializes some global variables */
ساكن UHashtable *SHARED_داده_HASHTABLE = اشاره_تهی;
ساكن icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

ساكن ثابت حرف **g_موجود_Converters = اشاره_تهی;
ساكن uint16_t g_موجود_Converter_شمار = 0;
ساكن icu::UInitOnce g_موجود_Converters_Init_Once {};

#إذا !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
ساكن حرف g_Default_Converter_Name_میانگیر[UCNV_MAX_CONVERTER_NAME_درازی + 1]; /* +1 for nullptr */
ساكن ثابت حرف *gDefaultConverterName = اشاره_تهی;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
ساكن ثابت UConverter_Shared_داده *g_Default_Algorithmic_Shared_داده = اشاره_تهی;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
ساكن UBool g_Default_Converter_شامل_است_Option;

#پایان_اگر  /* !U_CHARSET_IS_UTF8 */

ساكن ثابت حرف داده_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
ساكن فراغ
ucnv_flush_موجود_Converter_Cache() {
    g_موجود_Converter_شمار = 0;
    إذا (g_موجود_Converters) {
        uprv_free(const_cast<حرف**>(g_موجود_Converters));
        g_موجود_Converters = اشاره_تهی;
    }
    g_موجود_Converters_Init_Once.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
ساكن UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    إذا (SHARED_داده_HASHTABLE != اشاره_تهی && uhash_شمار(SHARED_داده_HASHTABLE) == 0) {
        uhash_close(SHARED_داده_HASHTABLE);
        SHARED_داده_HASHTABLE = اشاره_تهی;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flush_موجود_Converter_Cache();

#إذا !U_CHARSET_IS_UTF8
    gDefaultConverterName = اشاره_تهی;
    g_Default_Converter_Name_میانگیر[0] = 0;
    g_Default_Converter_شامل_است_Option = خطأ;
    g_Default_Algorithmic_Shared_داده = اشاره_تهی;
#پایان_اگر

    أرجع (SHARED_داده_HASHTABLE == اشاره_تهی);
}

U_CAPI فراغ U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_ثبات_Cleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
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
        pInfo->داده_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->داده_Format[1]==0x6e &&
        pInfo->داده_Format[2]==0x76 &&
        pInfo->داده_Format[3]==0x74 &&
        pInfo->format_نسخه[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
ساكن UConverter_Shared_داده*
ucnv_داده_un_Flatten_Clone(UConverter_بارگیری_آرگومانها *p_آرگومانها, UData_حافظه *p_داده, UError_کد *وضعیت)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ثابت uint8_t* raw = static_cast<ثابت uint8_t*>(udata_get_حافظه(p_داده));
    ثابت UConverter_Static_داده* منبع = reinterpret_cast<ثابت UConverter_Static_داده*>(raw);
    UConverter_Shared_داده *داده;
    UConverterType type = static_cast<UConverterType>(منبع->conversionType);

    إذا(U_FAILURE(*وضعیت))
        أرجع اشاره_تهی;

    إذا (static_cast<uint16_t>(type) >= UCNV_عدد_از_SUPPORTED_CONVERTER_TYPES ||
        converter_داده[type] == اشاره_تهی ||
        !converter_داده[type]->isReferenceCounted ||
        converter_داده[type]->referenceCounter != 1 ||
        منبع->struct_اندازه != حجم(UConverter_Static_داده))
    {
        *وضعیت = U_INVALID_جدول_FORMAT;
        أرجع اشاره_تهی;
    }

    داده = static_cast<UConverter_Shared_داده*>(uprv_malloc(حجم(UConverter_Shared_داده)));
    إذا(داده == اشاره_تهی) {
        *وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(داده, converter_داده[type], حجم(UConverter_Shared_داده));

    داده->static_داده = منبع;

    داده->shared_داده_Cached = خطأ;

    /* fill in fields from the loaded data */
    داده->داده_حافظه = (فراغ*)p_داده; /* for future use */

    إذا(داده->impl->load != اشاره_تهی) {
        داده->impl->load(داده, p_آرگومانها, raw + منبع->struct_اندازه, وضعیت);
        إذا(U_FAILURE(*وضعیت)) {
            uprv_free(داده);
            أرجع اشاره_تهی;
        }
    }
    أرجع داده;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
ساكن UConverter_Shared_داده *createConverterFromFile(UConverter_بارگیری_آرگومانها *p_آرگومانها, UError_کد * err)
{
    UData_حافظه *داده;
    UConverter_Shared_داده *shared_داده;

    UTRACE_مدخل_OC(UTRACE_UCNV_بارگیری);

    إذا (U_FAILURE (*err)) {
        UTRACE_EXIT_وضعیت(*err);
        أرجع اشاره_تهی;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", p_آرگومانها->name, p_آرگومانها->pkg);

    داده = udata_openChoice(p_آرگومانها->pkg, داده_TYPE, p_آرگومانها->name, isCnvAcceptable, اشاره_تهی, err);
    إذا(U_FAILURE(*err))
    {
        UTRACE_EXIT_وضعیت(*err);
        أرجع اشاره_تهی;
    }

    shared_داده = ucnv_داده_un_Flatten_Clone(p_آرگومانها, داده, err);
    إذا(U_FAILURE(*err))
    {
        udata_close(داده);
        UTRACE_EXIT_وضعیت(*err);
        أرجع اشاره_تهی;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_وضعیت(shared_داده, *err);
    أرجع shared_داده;
}

/*returns a converter type from a string
 */
ساكن ثابت UConverter_Shared_داده *
getAlgorithmicTypeFromName(ثابت حرف *realName)
{
    uint32_t mid, شروع, حد;
    uint32_t آخرین_Mid;
    صحيح نتیجه;
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_درازی];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    شروع = 0;
    حد = UPRV_LENGTHOF(cnvNameType);
    mid = حد;
    آخرین_Mid = UINT32_MAX;

    لكل (;;) {
        mid = (شروع + حد) / 2;
        إذا (آخرین_Mid == mid) {   /* Have we moved? */
            اكسر;  /* We haven't moved, and it wasn't found. */
        }
        آخرین_Mid = mid;
        نتیجه = uprv_strcmp(strippedName, cnvNameType[mid].name);

        إذا (نتیجه < 0) {
            حد = mid;
        } وإلا إذا (نتیجه > 0) {
            شروع = mid;
        } وإلا {
            أرجع converter_داده[cnvNameType[mid].type];
        }
    }

    أرجع اشاره_تهی;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#تعریف_کن UCNV_CACHE_بارگیری_ضریب 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
ساكن فراغ
ucnv_share_Converter_داده(UConverter_Shared_داده * داده)
{
    UError_کد err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    إذا (SHARED_داده_HASHTABLE == اشاره_تهی)
    {
        SHARED_داده_HASHTABLE = uhash_open_اندازه(uhash_hashChars, uhash_compareChars, اشاره_تهی,
                            ucnv_io_شمار_معلوم_Converters(&err)*UCNV_CACHE_بارگیری_ضریب,
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
    داده->shared_داده_Cached = درست;

    uhash_put(SHARED_داده_HASHTABLE,
            (فراغ*) داده->static_داده->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            داده,
            &err);
    UCNV_DEBUG_LOG("put", داده->static_داده->name,داده);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
ساكن UConverter_Shared_داده *
ucnv_get_Shared_Converter_داده(ثابت حرف *name)
{
    /*special case when no Table has yet been created we return nullptr */
    إذا (SHARED_داده_HASHTABLE == اشاره_تهی)
    {
        أرجع اشاره_تهی;
    }
    وإلا
    {
        UConverter_Shared_داده *rc;

        rc = static_cast<UConverter_Shared_داده*>(uhash_get(SHARED_داده_HASHTABLE, name));
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
ucnv_delete_Shared_Converter_داده(UConverter_Shared_داده * dead_Shared_داده)
{
    UTRACE_مدخل_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_داده->static_داده->name, dead_Shared_داده);

    إذا (dead_Shared_داده->referenceCounter > 0) {
        UTRACE_EXIT_مقدار((int32_t)خطأ);
        أرجع خطأ;
    }

    إذا (dead_Shared_داده->impl->unload != اشاره_تهی) {
        dead_Shared_داده->impl->unload(dead_Shared_داده);
    }

    إذا(dead_Shared_داده->داده_حافظه != اشاره_تهی)
    {
        UData_حافظه *داده = (UData_حافظه*)dead_Shared_داده->داده_حافظه;
        udata_close(داده);
    }

    uprv_free(dead_Shared_داده);

    UTRACE_EXIT_مقدار((int32_t)درست);
    أرجع درست;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_داده *
ucnv_بارگیری(UConverter_بارگیری_آرگومانها *p_آرگومانها, UError_کد *err) {
    UConverter_Shared_داده *my_Shared_Converter_داده;

    إذا(err == اشاره_تهی || U_FAILURE(*err)) {
        أرجع اشاره_تهی;
    }

    إذا(p_آرگومانها->pkg != اشاره_تهی && *p_آرگومانها->pkg != 0) {
        /* application-provided converters are not currently cached */
        أرجع createConverterFromFile(p_آرگومانها, err);
    }

    my_Shared_Converter_داده = ucnv_get_Shared_Converter_داده(p_آرگومانها->name);
    إذا (my_Shared_Converter_داده == اشاره_تهی)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_داده = createConverterFromFile(p_آرگومانها, err);
        إذا (U_FAILURE (*err) || (my_Shared_Converter_داده == اشاره_تهی))
        {
            أرجع اشاره_تهی;
        }
        وإلا إذا (!p_آرگومانها->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_داده(my_Shared_Converter_داده);
        }
    }
    وإلا
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_داده->referenceCounter++;
    }

    أرجع my_Shared_Converter_داده;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI فراغ
ucnv_unload(UConverter_Shared_داده *shared_داده) {
    إذا(shared_داده != اشاره_تهی) {
        إذا (shared_داده->referenceCounter > 0) {
            shared_داده->referenceCounter--;
        }

        إذا((shared_داده->referenceCounter <= 0)&&(shared_داده->shared_داده_Cached == خطأ)) {
            ucnv_delete_Shared_Converter_داده(shared_داده);
        }
    }
}

U_CFUNC فراغ
ucnv_unload_Shared_داده_If_Ready(UConverter_Shared_داده *shared_داده)
{
    إذا(shared_داده != اشاره_تهی && shared_داده->isReferenceCounted) {
        umtx_قفل(&cnvCacheMutex);
        ucnv_unload(shared_داده);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC فراغ
ucnv_increment_Ref_شمار(UConverter_Shared_داده *shared_داده)
{
    إذا(shared_داده != اشاره_تهی && shared_داده->isReferenceCounted) {
        umtx_قفل(&cnvCacheMutex);
        shared_داده->referenceCounter++;
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
parseConverterOptions(ثابت حرف *درون_Name,
                      UConverterNamePieces *pPieces,
                      UConverter_بارگیری_آرگومانها *p_آرگومانها,
                      UError_کد *err)
{
    حرف *cnvName = pPieces->cnvName;
    حرف c;
    int32_t len = 0;

    p_آرگومانها->name=درون_Name;
    p_آرگومانها->locale=pPieces->locale;
    p_آرگومانها->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    طالما((c=*درون_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        إذا (++len>=UCNV_MAX_CONVERTER_NAME_درازی) {
            *err = U_ILLEGAL_آرگومان_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            أرجع;
        }
        *cnvName++=c;
        درون_Name++;
    }
    *cnvName=0;
    p_آرگومانها->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    طالما((c=*درون_Name)!=0) {
        إذا(c==UCNV_OPTION_SEP_CHAR) {
            ++درون_Name;
        }

        /* inName is behind an option separator */
        إذا(uprv_strncmp(درون_Name, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            حرف *dest=pPieces->locale;

            /* copy the locale option value */
            درون_Name+=7;
            len=0;
            طالما((c=*درون_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++درون_Name;

                إذا(++len>=ULOC_FULLNAME_ظرفیت) {
                    *err=U_ILLEGAL_آرگومان_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    أرجع;
                }

                *dest++=c;
            }
            *dest=0;
        } وإلا إذا(uprv_strncmp(درون_Name, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            درون_Name+=8;
            c=*درون_Name;
            إذا(c==0) {
                p_آرگومانها->options=(pPieces->options&=~UCNV_OPTION_نسخه);
                أرجع;
            } وإلا إذا (static_cast<uint8_t>(c - '0') < 10) {
                p_آرگومانها->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_نسخه) | static_cast<uint32_t>(c - '0');
                ++درون_Name;
            }
        } وإلا إذا(uprv_strncmp(درون_Name, "swaplfnl", 8)==0) {
            درون_Name+=8;
            p_آرگومانها->options=(pPieces->options|=UCNV_OPTION_جابجا_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } وإلا {
            /* ignore any other options until we define some */
            طالما(((c = *درون_Name++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
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
U_CFUNC UConverter_Shared_داده *
ucnv_بارگیری_Shared_داده(ثابت حرف *converterName,
                    UConverterNamePieces *pPieces,
                    UConverter_بارگیری_آرگومانها *p_آرگومانها,
                    UError_کد * err) {
    UConverterNamePieces پشته_Pieces;
    UConverter_بارگیری_آرگومانها پشته_آرگومانها;
    UConverter_Shared_داده *my_Shared_Converter_داده = اشاره_تهی;
    UError_کد داخلی_Error_کد = U_ZERO_ERROR;
    UBool mayContainOption = درست;
    UBool checkForAlgorithmic = درست;

    إذا (U_FAILURE (*err)) {
        أرجع اشاره_تهی;
    }

    إذا(pPieces == اشاره_تهی) {
        إذا(p_آرگومانها != اشاره_تهی) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_داخلی_برنامه_ERROR;
            أرجع اشاره_تهی;
        }
        pPieces = &پشته_Pieces;
    }
    إذا(p_آرگومانها == اشاره_تهی) {
        uprv_memset(&پشته_آرگومانها, 0, حجم(پشته_آرگومانها));
        پشته_آرگومانها.size = (int32_t)حجم(پشته_آرگومانها);
        p_آرگومانها = &پشته_آرگومانها;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    p_آرگومانها->name = converterName;
    p_آرگومانها->locale = pPieces->locale;
    p_آرگومانها->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    إذا (converterName == اشاره_تهی) {
#إذا U_CHARSET_IS_UTF8
        p_آرگومانها->name = "UTF-8";
        أرجع (UConverter_Shared_داده *)converter_داده[UCNV_UTF8];
#وإلا
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        p_آرگومانها->name = ucnv_getDefaultName();
        إذا (p_آرگومانها->name == اشاره_تهی) {
            *err = U_MISSING_RESOURCE_ERROR;
            أرجع اشاره_تهی;
        }
        my_Shared_Converter_داده = (UConverter_Shared_داده *)g_Default_Algorithmic_Shared_داده;
        checkForAlgorithmic = خطأ;
        mayContainOption = g_Default_Converter_شامل_است_Option;
        /* the default converter name is already canonical */
#پایان_اگر
    }
    وإلا إذا(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        p_آرگومانها->name = "UTF-8";
        أرجع (UConverter_Shared_داده *)converter_داده[UCNV_UTF8];
    }
    وإلا {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, p_آرگومانها, err);
        إذا (U_FAILURE(*err)) {
            /* Very bad name used. */
            أرجع اشاره_تهی;
        }

        /* get the canonical converter name */
        p_آرگومانها->name = ucnv_io_getConverterName(p_آرگومانها->name, &mayContainOption, &داخلی_Error_کد);
        إذا (U_FAILURE(داخلی_Error_کد) || p_آرگومانها->name == اشاره_تهی) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            p_آرگومانها->name = pPieces->cnvName;
        } وإلا إذا (داخلی_Error_کد == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    إذا(mayContainOption && p_آرگومانها->name != pPieces->cnvName) {
        parseConverterOptions(p_آرگومانها->name, pPieces, p_آرگومانها, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    إذا (checkForAlgorithmic) {
        my_Shared_Converter_داده = (UConverter_Shared_داده *)getAlgorithmicTypeFromName(p_آرگومانها->name);
    }
    إذا (my_Shared_Converter_داده == اشاره_تهی)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        p_آرگومانها->nestedLoads=1;
        p_آرگومانها->pkg=اشاره_تهی;

        umtx_قفل(&cnvCacheMutex);
        my_Shared_Converter_داده = ucnv_بارگیری(p_آرگومانها, err);
        umtx_unlock(&cnvCacheMutex);
        إذا (U_FAILURE (*err) || (my_Shared_Converter_داده == اشاره_تهی))
        {
            أرجع اشاره_تهی;
        }
    }

    أرجع my_Shared_Converter_داده;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ثابت حرف* converterName,
                     UError_کد* err)
{
    UConverterNamePieces پشته_Pieces;
    UConverter_بارگیری_آرگومانها پشته_آرگومانها=UCNV_بارگیری_آرگومانها_INITIALIZER;
    UConverter_Shared_داده *my_Shared_Converter_داده;

    UTRACE_مدخل_OC(UTRACE_UCNV_OPEN);

    إذا(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_داده = ucnv_بارگیری_Shared_داده(converterName, &پشته_Pieces, &پشته_آرگومانها, err);

        myUConverter = ucnv_create_Converter_From_Shared_داده(
            myUConverter, my_Shared_Converter_داده,
            &پشته_آرگومانها,
            err);

        إذا(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_وضعیت(myUConverter, *err);
            أرجع myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_وضعیت(*err);
    أرجع اشاره_تهی;
}

U_CFUNC UBool
ucnv_canCreateConverter(ثابت حرف *converterName, UError_کد *err) {
    UConverter myUConverter;
    UConverterNamePieces پشته_Pieces;
    UConverter_بارگیری_آرگومانها پشته_آرگومانها=UCNV_بارگیری_آرگومانها_INITIALIZER;
    UConverter_Shared_داده *my_Shared_Converter_داده;

    UTRACE_مدخل_OC(UTRACE_UCNV_OPEN);

    إذا(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        پشته_آرگومانها.onlyTestIsLoadable=درست;
        my_Shared_Converter_داده = ucnv_بارگیری_Shared_داده(converterName, &پشته_Pieces, &پشته_آرگومانها, err);
        ucnv_create_Converter_From_Shared_داده(
            &myUConverter, my_Shared_Converter_داده,
            &پشته_آرگومانها,
            err);
        ucnv_unload_Shared_داده_If_Ready(my_Shared_Converter_داده);
    }

    UTRACE_EXIT_وضعیت(*err);
    أرجع U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ثابت حرف* locale,
                                uint32_t options,
                                UError_کد* err) {
    UConverter *cnv;
    ثابت UConverter_Shared_داده *shared_داده;
    UConverter_بارگیری_آرگومانها پشته_آرگومانها=UCNV_بارگیری_آرگومانها_INITIALIZER;

    UTRACE_مدخل_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    إذا(type<0 || UCNV_عدد_از_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_آرگومان_ERROR;
        UTRACE_EXIT_وضعیت(U_ILLEGAL_آرگومان_ERROR);
        أرجع اشاره_تهی;
    }

    shared_داده = converter_داده[type];
    إذا(shared_داده == اشاره_تهی || shared_داده->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_آرگومان_ERROR;
        UTRACE_EXIT_وضعیت(U_ILLEGAL_آرگومان_ERROR);
        أرجع اشاره_تهی;
    }

    پشته_آرگومانها.name = "";
    پشته_آرگومانها.options = options;
    پشته_آرگومانها.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_داده(
            myUConverter, const_cast<UConverter_Shared_داده*>(shared_داده),
            &پشته_آرگومانها, err);

    UTRACE_EXIT_PTR_وضعیت(cnv, *err);
    أرجع cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ثابت حرف *packageName, ثابت حرف *converterName, UError_کد * err)
{
    UConverter *myUConverter;
    UConverter_Shared_داده *my_Shared_Converter_داده;
    UConverterNamePieces پشته_Pieces;
    UConverter_بارگیری_آرگومانها پشته_آرگومانها=UCNV_بارگیری_آرگومانها_INITIALIZER;

    UTRACE_مدخل_OC(UTRACE_UCNV_OPEN_PACKAGE);

    إذا(U_FAILURE(*err)) {
        UTRACE_EXIT_وضعیت(*err);
        أرجع اشاره_تهی;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    پشته_Pieces.cnvName[0] = 0;
    پشته_Pieces.locale[0] = 0;
    پشته_Pieces.options = 0;
    parseConverterOptions(converterName, &پشته_Pieces, &پشته_آرگومانها, err);
    إذا (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_وضعیت(*err);
        أرجع اشاره_تهی;
    }
    پشته_آرگومانها.nestedLoads=1;
    پشته_آرگومانها.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_داده = createConverterFromFile(&پشته_آرگومانها, err);

    إذا (U_FAILURE(*err)) {
        UTRACE_EXIT_وضعیت(*err);
        أرجع اشاره_تهی;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_داده(اشاره_تهی, my_Shared_Converter_داده, &پشته_آرگومانها, err);

    إذا (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_وضعیت(*err);
        أرجع اشاره_تهی;
    }

    UTRACE_EXIT_PTR_وضعیت(myUConverter, *err);
    أرجع myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_داده(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_داده* my_Shared_Converter_داده,
                                   UConverter_بارگیری_آرگومانها* p_آرگومانها,
                                   UError_کد* err)
{
    UBool isCopyLocal;

    إذا(U_FAILURE(*err)) {
        ucnv_unload_Shared_داده_If_Ready(my_Shared_Converter_داده);
        أرجع myUConverter;
    }
    إذا(myUConverter == اشاره_تهی)
    {
        myUConverter = (UConverter *) uprv_malloc (حجم (UConverter));
        إذا(myUConverter == اشاره_تهی)
        {
            *err = U_حافظه_تخصیص_ERROR;
            ucnv_unload_Shared_داده_If_Ready(my_Shared_Converter_داده);
            أرجع اشاره_تهی;
        }
        isCopyLocal = خطأ;
    } وإلا {
        isCopyLocal = درست;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, حجم(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_داده = my_Shared_Converter_داده;
    myUConverter->options = p_آرگومانها->options;
    إذا(!p_آرگومانها->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_مقصد_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->مقصد_Unicode_وضعیت = my_Shared_Converter_داده->مقصد_Unicode_وضعیت;
        myUConverter->max_بایتها_در_هر_UChar = my_Shared_Converter_داده->static_داده->max_بایتها_در_هر_Char;
        myUConverter->subChar1 = my_Shared_Converter_داده->static_داده->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_داده->static_داده->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_داده->static_داده->subChar, myUConverter->subCharLen);
        myUConverter->مقصد_UCallback_Reason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    إذا(my_Shared_Converter_داده->impl->open != اشاره_تهی) {
        my_Shared_Converter_داده->impl->open(myUConverter, p_آرگومانها, err);
        إذا(U_FAILURE(*err) && !p_آرگومانها->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            أرجع اشاره_تهی;
        }
    }

    أرجع myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_داده *my_Shared_داده = اشاره_تهی;
    int32_t pos;
    int32_t جدول_Deleted_رقم = 0;
    ثابت UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, مانده;

    UTRACE_مدخل_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    إذا (SHARED_داده_HASHTABLE == اشاره_تهی) {
        UTRACE_EXIT_مقدار((int32_t)0);
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
    umtx_قفل(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    افعل {
        مانده = 0;
        pos = UHASH_اولی;
        طالما ((e = uhash_بعدی_Element (SHARED_داده_HASHTABLE, &pos)) != اشاره_تهی)
        {
            my_Shared_داده = (UConverter_Shared_داده *) e->value.اشاره_گر;
            /*deletes only if reference counter == 0 */
            إذا (my_Shared_داده->referenceCounter == 0)
            {
                جدول_Deleted_رقم++;

                UCNV_DEBUG_LOG("del",my_Shared_داده->static_داده->name,my_Shared_داده);

                uhash_removeElement(SHARED_داده_HASHTABLE, e);
                my_Shared_داده->shared_داده_Cached = خطأ;
                ucnv_delete_Shared_Converter_داده (my_Shared_داده);
            } وإلا {
                ++مانده;
            }
        }
    } طالما(++i == 1 && مانده > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", مانده);

    UTRACE_EXIT_مقدار(جدول_Deleted_رقم);
    أرجع جدول_Deleted_رقم;
}

/* available converters list --------------------------------------------------- */

ساكن فراغ U_CALLCONV init_موجود_Converters_List(UError_کد &err_کد) {
    U_ASSERT(g_موجود_Converter_شمار == 0);
    U_ASSERT(g_موجود_Converters == اشاره_تهی);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_open_All_نامها(&err_کد);
    int32_t all_Converter_شمار = uenum_شمار(allConvEnum, &err_کد);
    إذا (U_FAILURE(err_کد)) {
        أرجع;
    }

    /* We can't have more than "*converterTable" converters to open */
    g_موجود_Converters = static_cast<ثابت حرف**>(uprv_malloc(all_Converter_شمار * حجم(حرف*)));
    إذا (!g_موجود_Converters) {
        err_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UError_کد local_وضعیت = U_ZERO_ERROR;
    UConverter موقت_Converter;
    ucnv_close(ucnv_createConverter(&موقت_Converter, اشاره_تهی, &local_وضعیت));

    g_موجود_Converter_شمار = 0;

    لكل (int32_t idx = 0; idx < all_Converter_شمار; idx++) {
        local_وضعیت = U_ZERO_ERROR;
        ثابت حرف *converterName = uenum_بعدی(allConvEnum, اشاره_تهی, &local_وضعیت);
        إذا (ucnv_canCreateConverter(converterName, &local_وضعیت)) {
            g_موجود_Converters[g_موجود_Converter_شمار++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


ساكن UBool have_موجود_Converter_List(UError_کد *p_Error_کد) {
    umtx_initOnce(g_موجود_Converters_Init_Once, &init_موجود_Converters_List, *p_Error_کد);
    أرجع U_SUCCESS(*p_Error_کد);
}

U_CFUNC uint16_t
ucnv_bld_شمار_موجود_Converters(UError_کد *p_Error_کد) {
    إذا (have_موجود_Converter_List(p_Error_کد)) {
        أرجع g_موجود_Converter_شمار;
    }
    أرجع 0;
}

U_CFUNC ثابت حرف *
ucnv_bld_get_موجود_Converter(uint16_t n, UError_کد *p_Error_کد) {
    إذا (have_موجود_Converter_List(p_Error_کد)) {
        إذا (n < g_موجود_Converter_شمار) {
            أرجع g_موجود_Converters[n];
        }
        *p_Error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
    }
    أرجع اشاره_تهی;
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
داخلی_مجموعه_Name(ثابت حرف *name, UError_کد *وضعیت) {
    UConverterNamePieces پشته_Pieces;
    UConverter_بارگیری_آرگومانها پشته_آرگومانها=UCNV_بارگیری_آرگومانها_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool شامل_است_Option = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != اشاره_تهی);
    ثابت UConverter_Shared_داده *algorithmic_Shared_داده;

    پشته_آرگومانها.name = name;
    إذا(شامل_است_Option) {
        پشته_Pieces.cnvName[0] = 0;
        پشته_Pieces.locale[0] = 0;
        پشته_Pieces.options = 0;
        parseConverterOptions(name, &پشته_Pieces, &پشته_آرگومانها, وضعیت);
        إذا(U_FAILURE(*وضعیت)) {
            أرجع;
        }
    }
    algorithmic_Shared_داده = getAlgorithmicTypeFromName(پشته_آرگومانها.name);

    umtx_قفل(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_داده = algorithmic_Shared_داده;
    g_Default_Converter_شامل_است_Option = شامل_است_Option;
    uprv_memcpy(g_Default_Converter_Name_میانگیر, name, length);
    g_Default_Converter_Name_میانگیر[length]=0;

    /* gDefaultConverterName MUST be the last global var set by this function.  */
    /*    It is the variable checked in ucnv_getDefaultName() to see if initialization is required. */
    //    But there is nothing here preventing that from being reordered, either by the compiler
    //             or hardware. I'm adding the mutex to ucnv_getDefaultName for now. UMTX_CHECK is not enough.
    //             -- Andy
    gDefaultConverterName = g_Default_Converter_Name_میانگیر;

    ucnv_enableCleanup();

    umtx_unlock(&cnvCacheMutex);
}
#پایان_اگر

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
        icu::Mutex قفل(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    إذا(name==اشاره_تهی) {
        UError_کد error_کد = U_ZERO_ERROR;
        UConverter *cnv = اشاره_تهی;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        إذا(name != اشاره_تهی) {
            cnv = ucnv_open(name, &error_کد);
            إذا(U_SUCCESS(error_کد) && cnv != اشاره_تهی) {
                name = ucnv_getName(cnv, &error_کد);
            }
        }

        إذا(name == اشاره_تهی || name[0] == 0
            || U_FAILURE(error_کد) || cnv == اشاره_تهی
            || uprv_strlen(name)>=حجم(g_Default_Converter_Name_میانگیر))
        {
            /* Panic time, let's use a fallback. */
#إذا (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#وإلاإذا U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_جابجا_LFNL_OPTION_STRING;
#وإلا
            name = "ibm-37_P100-1995";
#پایان_اگر
        }

        داخلی_مجموعه_Name(name, &error_کد);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    أرجع name;
#پایان_اگر
}

#إذا U_CHARSET_IS_UTF8
U_CAPI فراغ U_EXPORT2 ucnv_مجموعه_Default_Name(ثابت حرف *) {}
#وإلا
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI فراغ U_EXPORT2
ucnv_مجموعه_Default_Name(ثابت حرف *converterName) {
    إذا(converterName==اشاره_تهی) {
        /* reset to the default codepage */
        gDefaultConverterName=اشاره_تهی;
    } وإلا {
        UError_کد error_کد = U_ZERO_ERROR;
        UConverter *cnv = اشاره_تهی;
        ثابت حرف *name = اشاره_تهی;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &error_کد);
        إذا(U_SUCCESS(error_کد) && cnv != اشاره_تهی) {
            name = ucnv_getName(cnv, &error_کد);
        }

        إذا(U_SUCCESS(error_کد) && name!=اشاره_تهی) {
            داخلی_مجموعه_Name(name, &error_کد);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#پایان_اگر

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#إذا !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_جابجا(ثابت UDataSwapper *ds,
          ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
          UError_کد *p_Error_کد) {
    ثابت UDataInfo *pInfo;
    int32_t سرآیند_اندازه;

    ثابت uint8_t *درون_بایتها;
    uint8_t *بیرون_بایتها;

    uint32_t جابه_جایی, count, static_داده_اندازه;
    int32_t size;

    ثابت UConverter_Static_داده *درون_Static_داده;
    UConverter_Static_داده *بیرون_Static_داده;

    ثابت _MBCSHeader *درون_MBCSHeader;
    _MBCSHeader *بیرون_MBCSHeader;
    _MBCSHeader mbcs_سرآیند;
    uint32_t mbcs_سرآیند_درازی;
    UBool noFromU=خطأ;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_شاخص_درازی;

    ثابت int32_t *درون_Ext_Indexes;
    int32_t ext_جابه_جایی;

    /* udata_swapDataHeader checks the arguments */
    سرآیند_اندازه=udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)درون_داده+4);
    إذا(!(
        pInfo->داده_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->داده_Format[1]==0x6e &&
        pInfo->داده_Format[2]==0x76 &&
        pInfo->داده_Format[3]==0x74 &&
        pInfo->format_نسخه[0]==6 &&
        pInfo->format_نسخه[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->داده_Format[0], pInfo->داده_Format[1],
                         pInfo->داده_Format[2], pInfo->داده_Format[3],
                         pInfo->format_نسخه[0], pInfo->format_نسخه[1]);
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    درون_بایتها=(ثابت uint8_t *)درون_داده+سرآیند_اندازه;
    بیرون_بایتها=(بیرون_داده == اشاره_تهی) ? اشاره_تهی : (uint8_t *)بیرون_داده+سرآیند_اندازه;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    درون_Static_داده=(ثابت UConverter_Static_داده *)درون_بایتها;
    بیرون_Static_داده=(UConverter_Static_داده *)بیرون_بایتها;

    إذا(length<0) {
        static_داده_اندازه=ds->readUInt32(درون_Static_داده->struct_اندازه);
    } وإلا {
        length-=سرآیند_اندازه;
        إذا( length<(int32_t)حجم(UConverter_Static_داده) ||
            (uint32_t)length<(static_داده_اندازه=ds->readUInt32(درون_Static_داده->struct_اندازه))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    إذا(length>=0) {
        /* swap the static data */
        إذا(درون_Static_داده!=بیرون_Static_داده) {
            uprv_memcpy(بیرون_Static_داده, درون_Static_داده, static_داده_اندازه);
        }

        ds->جابجا_Array32(ds, &درون_Static_داده->struct_اندازه, 4,
                           &بیرون_Static_داده->struct_اندازه, p_Error_کد);
        ds->جابجا_Array32(ds, &درون_Static_داده->codepage, 4,
                           &بیرون_Static_داده->codepage, p_Error_کد);

        ds->جابجا_Inv_Chars(ds, درون_Static_داده->name, (int32_t)uprv_strlen(درون_Static_داده->name),
                            بیرون_Static_داده->name, p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            أرجع 0;
        }
    }

    درون_بایتها+=static_داده_اندازه;
    إذا (بیرون_بایتها != اشاره_تهی) بیرون_بایتها+=static_داده_اندازه;
    إذا(length>=0) {
        length-=(int32_t)static_داده_اندازه;
    }

    /* check for supported conversionType values */
    إذا(درون_Static_داده->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        درون_MBCSHeader=(ثابت _MBCSHeader *)درون_بایتها;
        بیرون_MBCSHeader=(_MBCSHeader *)بیرون_بایتها;

        إذا(0<=length && length<(int32_t)حجم(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        إذا(درون_MBCSHeader->نسخه[0]==4 && درون_MBCSHeader->نسخه[1]>=1) {
            mbcs_سرآیند_درازی=MBCS_سرآیند_V4_درازی;
        } وإلا إذا(درون_MBCSHeader->نسخه[0]==5 && درون_MBCSHeader->نسخه[1]>=3 &&
                  ((mbcs_سرآیند.options=ds->readUInt32(درون_MBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_ماسک)==0
        ) {
            mbcs_سرآیند_درازی=mbcs_سرآیند.options&MBCS_OPT_درازی_ماسک;
            noFromU = (mbcs_سرآیند.options & MBCS_OPT_NO_FROM_U) != 0;
        } وإلا {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             درون_MBCSHeader->نسخه[0], درون_MBCSHeader->نسخه[1]);
            *p_Error_کد=U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        uprv_memcpy(mbcs_سرآیند.نسخه, درون_MBCSHeader->نسخه, 4);
        mbcs_سرآیند.شمار_حالتها=         ds->readUInt32(درون_MBCSHeader->شمار_حالتها);
        mbcs_سرآیند.شمار_مقصد_UFallbacks=   ds->readUInt32(درون_MBCSHeader->شمار_مقصد_UFallbacks);
        mbcs_سرآیند.جابه_جایی_مقصد_UCode_Units=  ds->readUInt32(درون_MBCSHeader->جابه_جایی_مقصد_UCode_Units);
        mbcs_سرآیند.جابه_جایی_From_UTable=    ds->readUInt32(درون_MBCSHeader->جابه_جایی_From_UTable);
        mbcs_سرآیند.جابه_جایی_From_UBytes=    ds->readUInt32(درون_MBCSHeader->جابه_جایی_From_UBytes);
        mbcs_سرآیند.flags=               ds->readUInt32(درون_MBCSHeader->flags);
        mbcs_سرآیند.from_UBytes_درازی=    ds->readUInt32(درون_MBCSHeader->from_UBytes_درازی);
        /* mbcsHeader.options have been read above */

        ext_جابه_جایی=(int32_t)(mbcs_سرآیند.flags>>8);
        outputType=(uint8_t)mbcs_سرآیند.flags;
        إذا(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *p_Error_کد=U_UNSUPPORTED_ERROR;
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
            *p_Error_کد=U_UNSUPPORTED_ERROR;
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
        mbcs_شاخص_درازی=0;
        إذا( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcs_سرآیند.نسخه[1]>=3 && (maxFastUChar=mbcs_سرآیند.نسخه[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_شاخص_درازی=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        إذا(ext_جابه_جایی==0) {
            size=(int32_t)(mbcs_سرآیند.جابه_جایی_From_UBytes+mbcs_شاخص_درازی);
            إذا(!noFromU) {
                size+=(int32_t)mbcs_سرآیند.from_UBytes_درازی;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            درون_Ext_Indexes=اشاره_تهی;
        } وإلا {
            /* there is extension data after the base data, see ucnv_ext.h */
            إذا(length>=0 && length<(ext_جابه_جایی+UCNV_EXT_INDEXES_MIN_درازی*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            درون_Ext_Indexes=(ثابت int32_t *)(درون_بایتها+ext_جابه_جایی);
            size=ext_جابه_جایی+udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_اندازه]);
        }

        إذا(length>=0) {
            إذا(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            /* copy the data for inaccessible bytes */
            إذا(درون_بایتها!=بیرون_بایتها) {
                uprv_memcpy(بیرون_بایتها, درون_بایتها, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_سرآیند_درازی*4;
            ds->جابجا_Array32(ds, &درون_MBCSHeader->شمار_حالتها, count-4,
                               &بیرون_MBCSHeader->شمار_حالتها, p_Error_کد);

            إذا(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ثابت حرف *درون_پایه_Name=(ثابت حرف *)درون_بایتها+count;
                حرف *بیرون_پایه_Name=(حرف *)بیرون_بایتها+count;
                ds->جابجا_Inv_Chars(ds, درون_پایه_Name, (int32_t)uprv_strlen(درون_پایه_Name),
                                    بیرون_پایه_Name, p_Error_کد);
            } وإلا {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                جابه_جایی=count;
                count=mbcs_سرآیند.شمار_حالتها*1024;
                ds->جابجا_Array32(ds, درون_بایتها+جابه_جایی, (int32_t)count,
                                   بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* swap the toUFallbacks[] */
                جابه_جایی+=count;
                count=mbcs_سرآیند.شمار_مقصد_UFallbacks*8;
                ds->جابجا_Array32(ds, درون_بایتها+جابه_جایی, (int32_t)count,
                                   بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* swap the unicodeCodeUnits[] */
                جابه_جایی=mbcs_سرآیند.جابه_جایی_مقصد_UCode_Units;
                count=mbcs_سرآیند.جابه_جایی_From_UTable-جابه_جایی;
                ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, (int32_t)count,
                                   بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* offset to the stage 1 table, independent of the outputType */
                جابه_جایی=mbcs_سرآیند.جابه_جایی_From_UTable;

                إذا(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcs_سرآیند.جابه_جایی_From_UBytes-جابه_جایی)+mbcs_سرآیند.from_UBytes_درازی;
                    ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, (int32_t)count,
                                       بیرون_بایتها+جابه_جایی, p_Error_کد);
                } وإلا {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    إذا(درون_Static_داده->unicode_ماسک&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } وإلا {
                        count=0x40*2; /* only BMP */
                    }
                    ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, (int32_t)count,
                                       بیرون_بایتها+جابه_جایی, p_Error_کد);

                    /* stage 2 table: uint32_t[] */
                    جابه_جایی+=count;
                    count=mbcs_سرآیند.جابه_جایی_From_UBytes-جابه_جایی;
                    ds->جابجا_Array32(ds, درون_بایتها+جابه_جایی, (int32_t)count,
                                       بیرون_بایتها+جابه_جایی, p_Error_کد);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    جابه_جایی=mbcs_سرآیند.جابه_جایی_From_UBytes;
                    count= noFromU ? 0 : mbcs_سرآیند.from_UBytes_درازی;
                    بدل(outputType) {
                    حالة MBCS_OUTPUT_2:
                    حالة MBCS_OUTPUT_3_EUC:
                    حالة MBCS_OUTPUT_2_SISO:
                        ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, (int32_t)count,
                                           بیرون_بایتها+جابه_جایی, p_Error_کد);
                        اكسر;
                    حالة MBCS_OUTPUT_4:
                        ds->جابجا_Array32(ds, درون_بایتها+جابه_جایی, (int32_t)count,
                                           بیرون_بایتها+جابه_جایی, p_Error_کد);
                        اكسر;
                    افتراضي:
                        /* just uint8_t[], nothing to swap */
                        اكسر;
                    }

                    إذا(mbcs_شاخص_درازی!=0) {
                        جابه_جایی+=count;
                        count=mbcs_شاخص_درازی;
                        ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, (int32_t)count,
                                           بیرون_بایتها+جابه_جایی, p_Error_کد);
                    }
                }
            }

            إذا(ext_جابه_جایی!=0) {
                /* swap the extension data */
                درون_بایتها+=ext_جابه_جایی;
                بیرون_بایتها+=ext_جابه_جایی;

                /* swap toUTable[] */
                جابه_جایی=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_مقصد_U_شاخص]);
                length=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_مقصد_U_درازی]);
                ds->جابجا_Array32(ds, درون_بایتها+جابه_جایی, length*4, بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* swap toUUChars[] */
                جابه_جایی=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_مقصد_U_UCHARS_شاخص]);
                length=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_مقصد_U_UCHARS_درازی]);
                ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, length*2, بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* swap fromUTableUChars[] */
                جابه_جایی=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_FROM_U_UCHARS_شاخص]);
                length=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_FROM_U_درازی]);
                ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, length*2, بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* swap fromUTableValues[] */
                جابه_جایی=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_FROM_U_مقادیر_شاخص]);
                /* same length as for fromUTableUChars[] */
                ds->جابجا_Array32(ds, درون_بایتها+جابه_جایی, length*4, بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                جابه_جایی=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_شاخص]);
                length=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_درازی]);
                ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, length*2, بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* swap fromUStage3[] */
                جابه_جایی=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_شاخص]);
                length=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_درازی]);
                ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, length*2, بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* swap fromUStage3b[] */
                جابه_جایی=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_شاخص]);
                length=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_درازی]);
                ds->جابجا_Array32(ds, درون_بایتها+جابه_جایی, length*4, بیرون_بایتها+جابه_جایی, p_Error_کد);

                /* swap indexes[] */
                length=udata_readInt32(ds, درون_Ext_Indexes[UCNV_EXT_INDEXES_درازی]);
                ds->جابجا_Array32(ds, درون_بایتها, length*4, بیرون_بایتها, p_Error_کد);
            }
        }
    } وإلا {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         درون_Static_داده->conversionType);
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    أرجع سرآیند_اندازه+(int32_t)static_داده_اندازه+size;
}

#پایان_اگر /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#پایان_اگر
