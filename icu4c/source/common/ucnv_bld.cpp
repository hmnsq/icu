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
#تعریف_کرو UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#وإلا
# define UCNV_DEBUG_LOG(x,y,z)
#جے_انت

ساكن ثابت UConverter_Shared_انکڑے * ثابت
converter_انکڑے[UCNV_نمبر_دا_SUPPORTED_CONVERTER_TYPES]={
    خالی_سوچک, خالی_سوچک,

#إذا UCONFIG_NO_LEGACY_CONVERSION
    خالی_سوچک,
#وإلا
    &_MBCSData,
#جے_انت

    &Latin1_انکڑے,
    &UTF8_انکڑے, &_UTF16BEData, &_UTF16LEData,
#إذا UCONFIG_ONLY_HTML_CONVERSION
    خالی_سوچک, خالی_سوچک,
#وإلا
    &_UTF32BEData, &_UTF32LEData,
#جے_انت
    خالی_سوچک,

#إذا UCONFIG_NO_LEGACY_CONVERSION
    خالی_سوچک,
#وإلا
    &ISO2022_انکڑے,
#جے_انت

#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    خالی_سوچک, خالی_سوچک, خالی_سوچک, خالی_سوچک, خالی_سوچک, خالی_سوچک,
    خالی_سوچک, خالی_سوچک, خالی_سوچک, خالی_سوچک, خالی_سوچک, خالی_سوچک,
    خالی_سوچک,
#وإلا
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#جے_انت

#إذا UCONFIG_ONLY_HTML_CONVERSION
    خالی_سوچک,
#وإلا
    &_SCSUData,
#جے_انت


#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    خالی_سوچک,
#وإلا
    &_ISCIIData,
#جے_انت

    &_ASCIIData,
#إذا UCONFIG_ONLY_HTML_CONVERSION
    خالی_سوچک, خالی_سوچک, &UTF16_انکڑے, خالی_سوچک, خالی_سوچک, خالی_سوچک,
#وإلا
    &UTF7_انکڑے, &Bocu1_انکڑے, &UTF16_انکڑے, &UTF32_انکڑے, &CESU8_انکڑے, &_IMAPData,
#جے_انت

#إذا UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    خالی_سوچک,
#وإلا
    &Compound_متن_انکڑے
#جے_انت
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
#جے_انت
#إذا !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#جے_انت
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#جے_انت
#إذا !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#جے_انت
#إذا !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#جے_انت
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
#جے_انت
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#جے_انت
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
#جے_انت
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
#جے_انت
#جے_انت
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#جے_انت
  { "utf8", UCNV_UTF8 },
#إذا !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_متن}
#جے_انت
};


/*initializes some global variables */
ساكن UHashtable *SHARED_انکڑے_HASHTABLE = خالی_سوچک;
ساكن icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

ساكن ثابت حرف **g_دستیاب_Converters = خالی_سوچک;
ساكن uint16_t g_دستیاب_Converter_گنتی = 0;
ساكن icu::UInitOnce g_دستیاب_Converters_Init_Once {};

#إذا !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
ساكن حرف g_Default_Converter_Name_بفر[UCNV_MAX_CONVERTER_NAME_لمائی + 1]; /* +1 for nullptr */
ساكن ثابت حرف *gDefaultConverterName = خالی_سوچک;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
ساكن ثابت UConverter_Shared_انکڑے *g_Default_Algorithmic_Shared_انکڑے = خالی_سوچک;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
ساكن UBool g_Default_Converter_شامل_اے_Option;

#جے_انت  /* !U_CHARSET_IS_UTF8 */

ساكن ثابت حرف انکڑے_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
ساكن فراغ
ucnv_flush_دستیاب_Converter_Cache() {
    g_دستیاب_Converter_گنتی = 0;
    إذا (g_دستیاب_Converters) {
        uprv_free(const_cast<حرف**>(g_دستیاب_Converters));
        g_دستیاب_Converters = خالی_سوچک;
    }
    g_دستیاب_Converters_Init_Once.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
ساكن UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    إذا (SHARED_انکڑے_HASHTABLE != خالی_سوچک && uhash_گنتی(SHARED_انکڑے_HASHTABLE) == 0) {
        uhash_close(SHARED_انکڑے_HASHTABLE);
        SHARED_انکڑے_HASHTABLE = خالی_سوچک;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flush_دستیاب_Converter_Cache();

#إذا !U_CHARSET_IS_UTF8
    gDefaultConverterName = خالی_سوچک;
    g_Default_Converter_Name_بفر[0] = 0;
    g_Default_Converter_شامل_اے_Option = خطأ;
    g_Default_Algorithmic_Shared_انکڑے = خالی_سوچک;
#جے_انت

    أرجع (SHARED_انکڑے_HASHTABLE == خالی_سوچک);
}

U_CAPI فراغ U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
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
        pInfo->انکڑے_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->انکڑے_Format[1]==0x6e &&
        pInfo->انکڑے_Format[2]==0x76 &&
        pInfo->انکڑے_Format[3]==0x74 &&
        pInfo->format_نسخہ[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
ساكن UConverter_Shared_انکڑے*
ucnv_انکڑے_un_Flatten_Clone(UConverter_لدو_دلیلاں *p_دلیلاں, UData_حافظہ *p_انکڑے, UError_کوڈ *حالت)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ثابت uint8_t* raw = static_cast<ثابت uint8_t*>(udata_get_حافظہ(p_انکڑے));
    ثابت UConverter_Static_انکڑے* سروت = reinterpret_cast<ثابت UConverter_Static_انکڑے*>(raw);
    UConverter_Shared_انکڑے *انکڑے;
    UConverterType type = static_cast<UConverterType>(سروت->conversionType);

    إذا(U_FAILURE(*حالت))
        أرجع خالی_سوچک;

    إذا (static_cast<uint16_t>(type) >= UCNV_نمبر_دا_SUPPORTED_CONVERTER_TYPES ||
        converter_انکڑے[type] == خالی_سوچک ||
        !converter_انکڑے[type]->isReferenceCounted ||
        converter_انکڑے[type]->referenceCounter != 1 ||
        سروت->struct_ناپ != حجم(UConverter_Static_انکڑے))
    {
        *حالت = U_INVALID_جدول_FORMAT;
        أرجع خالی_سوچک;
    }

    انکڑے = static_cast<UConverter_Shared_انکڑے*>(uprv_malloc(حجم(UConverter_Shared_انکڑے)));
    إذا(انکڑے == خالی_سوچک) {
        *حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(انکڑے, converter_انکڑے[type], حجم(UConverter_Shared_انکڑے));

    انکڑے->static_انکڑے = سروت;

    انکڑے->shared_انکڑے_Cached = خطأ;

    /* fill in fields from the loaded data */
    انکڑے->انکڑے_حافظہ = (فراغ*)p_انکڑے; /* for future use */

    إذا(انکڑے->impl->load != خالی_سوچک) {
        انکڑے->impl->load(انکڑے, p_دلیلاں, raw + سروت->struct_ناپ, حالت);
        إذا(U_FAILURE(*حالت)) {
            uprv_free(انکڑے);
            أرجع خالی_سوچک;
        }
    }
    أرجع انکڑے;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
ساكن UConverter_Shared_انکڑے *createConverterFromFile(UConverter_لدو_دلیلاں *p_دلیلاں, UError_کوڈ * err)
{
    UData_حافظہ *انکڑے;
    UConverter_Shared_انکڑے *shared_انکڑے;

    UTRACE_اندراج_OC(UTRACE_UCNV_لدو);

    إذا (U_FAILURE (*err)) {
        UTRACE_EXIT_حالت(*err);
        أرجع خالی_سوچک;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", p_دلیلاں->name, p_دلیلاں->pkg);

    انکڑے = udata_openChoice(p_دلیلاں->pkg, انکڑے_TYPE, p_دلیلاں->name, isCnvAcceptable, خالی_سوچک, err);
    إذا(U_FAILURE(*err))
    {
        UTRACE_EXIT_حالت(*err);
        أرجع خالی_سوچک;
    }

    shared_انکڑے = ucnv_انکڑے_un_Flatten_Clone(p_دلیلاں, انکڑے, err);
    إذا(U_FAILURE(*err))
    {
        udata_close(انکڑے);
        UTRACE_EXIT_حالت(*err);
        أرجع خالی_سوچک;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_حالت(shared_انکڑے, *err);
    أرجع shared_انکڑے;
}

/*returns a converter type from a string
 */
ساكن ثابت UConverter_Shared_انکڑے *
getAlgorithmicTypeFromName(ثابت حرف *realName)
{
    uint32_t mid, شروع, حد_بندی;
    uint32_t آخری_Mid;
    صحيح نتیجہ;
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_لمائی];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    شروع = 0;
    حد_بندی = UPRV_LENGTHOF(cnvNameType);
    mid = حد_بندی;
    آخری_Mid = UINT32_MAX;

    لكل (;;) {
        mid = (شروع + حد_بندی) / 2;
        إذا (آخری_Mid == mid) {   /* Have we moved? */
            اكسر;  /* We haven't moved, and it wasn't found. */
        }
        آخری_Mid = mid;
        نتیجہ = uprv_strcmp(strippedName, cnvNameType[mid].name);

        إذا (نتیجہ < 0) {
            حد_بندی = mid;
        } وإلا إذا (نتیجہ > 0) {
            شروع = mid;
        } وإلا {
            أرجع converter_انکڑے[cnvNameType[mid].type];
        }
    }

    أرجع خالی_سوچک;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#تعریف_کرو UCNV_CACHE_لدو_ضارب 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
ساكن فراغ
ucnv_share_Converter_انکڑے(UConverter_Shared_انکڑے * انکڑے)
{
    UError_کوڈ err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    إذا (SHARED_انکڑے_HASHTABLE == خالی_سوچک)
    {
        SHARED_انکڑے_HASHTABLE = uhash_open_ناپ(uhash_hashChars, uhash_compareChars, خالی_سوچک,
                            ucnv_io_گنتی_معلوم_Converters(&err)*UCNV_CACHE_لدو_ضارب,
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
    انکڑے->shared_انکڑے_Cached = سچ;

    uhash_put(SHARED_انکڑے_HASHTABLE,
            (فراغ*) انکڑے->static_انکڑے->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            انکڑے,
            &err);
    UCNV_DEBUG_LOG("put", انکڑے->static_انکڑے->name,انکڑے);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
ساكن UConverter_Shared_انکڑے *
ucnv_get_Shared_Converter_انکڑے(ثابت حرف *name)
{
    /*special case when no Table has yet been created we return nullptr */
    إذا (SHARED_انکڑے_HASHTABLE == خالی_سوچک)
    {
        أرجع خالی_سوچک;
    }
    وإلا
    {
        UConverter_Shared_انکڑے *rc;

        rc = static_cast<UConverter_Shared_انکڑے*>(uhash_get(SHARED_انکڑے_HASHTABLE, name));
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
ucnv_delete_Shared_Converter_انکڑے(UConverter_Shared_انکڑے * dead_Shared_انکڑے)
{
    UTRACE_اندراج_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_انکڑے->static_انکڑے->name, dead_Shared_انکڑے);

    إذا (dead_Shared_انکڑے->referenceCounter > 0) {
        UTRACE_EXIT_قدر((int32_t)خطأ);
        أرجع خطأ;
    }

    إذا (dead_Shared_انکڑے->impl->unload != خالی_سوچک) {
        dead_Shared_انکڑے->impl->unload(dead_Shared_انکڑے);
    }

    إذا(dead_Shared_انکڑے->انکڑے_حافظہ != خالی_سوچک)
    {
        UData_حافظہ *انکڑے = (UData_حافظہ*)dead_Shared_انکڑے->انکڑے_حافظہ;
        udata_close(انکڑے);
    }

    uprv_free(dead_Shared_انکڑے);

    UTRACE_EXIT_قدر((int32_t)سچ);
    أرجع سچ;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_انکڑے *
ucnv_لدو(UConverter_لدو_دلیلاں *p_دلیلاں, UError_کوڈ *err) {
    UConverter_Shared_انکڑے *my_Shared_Converter_انکڑے;

    إذا(err == خالی_سوچک || U_FAILURE(*err)) {
        أرجع خالی_سوچک;
    }

    إذا(p_دلیلاں->pkg != خالی_سوچک && *p_دلیلاں->pkg != 0) {
        /* application-provided converters are not currently cached */
        أرجع createConverterFromFile(p_دلیلاں, err);
    }

    my_Shared_Converter_انکڑے = ucnv_get_Shared_Converter_انکڑے(p_دلیلاں->name);
    إذا (my_Shared_Converter_انکڑے == خالی_سوچک)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_انکڑے = createConverterFromFile(p_دلیلاں, err);
        إذا (U_FAILURE (*err) || (my_Shared_Converter_انکڑے == خالی_سوچک))
        {
            أرجع خالی_سوچک;
        }
        وإلا إذا (!p_دلیلاں->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_انکڑے(my_Shared_Converter_انکڑے);
        }
    }
    وإلا
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_انکڑے->referenceCounter++;
    }

    أرجع my_Shared_Converter_انکڑے;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI فراغ
ucnv_unload(UConverter_Shared_انکڑے *shared_انکڑے) {
    إذا(shared_انکڑے != خالی_سوچک) {
        إذا (shared_انکڑے->referenceCounter > 0) {
            shared_انکڑے->referenceCounter--;
        }

        إذا((shared_انکڑے->referenceCounter <= 0)&&(shared_انکڑے->shared_انکڑے_Cached == خطأ)) {
            ucnv_delete_Shared_Converter_انکڑے(shared_انکڑے);
        }
    }
}

U_CFUNC فراغ
ucnv_unload_Shared_انکڑے_If_Ready(UConverter_Shared_انکڑے *shared_انکڑے)
{
    إذا(shared_انکڑے != خالی_سوچک && shared_انکڑے->isReferenceCounted) {
        umtx_جندرا(&cnvCacheMutex);
        ucnv_unload(shared_انکڑے);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC فراغ
ucnv_increment_Ref_گنتی(UConverter_Shared_انکڑے *shared_انکڑے)
{
    إذا(shared_انکڑے != خالی_سوچک && shared_انکڑے->isReferenceCounted) {
        umtx_جندرا(&cnvCacheMutex);
        shared_انکڑے->referenceCounter++;
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
parseConverterOptions(ثابت حرف *وچ_Name,
                      UConverterNamePieces *pPieces,
                      UConverter_لدو_دلیلاں *p_دلیلاں,
                      UError_کوڈ *err)
{
    حرف *cnvName = pPieces->cnvName;
    حرف c;
    int32_t len = 0;

    p_دلیلاں->name=وچ_Name;
    p_دلیلاں->locale=pPieces->locale;
    p_دلیلاں->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    طالما((c=*وچ_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        إذا (++len>=UCNV_MAX_CONVERTER_NAME_لمائی) {
            *err = U_ILLEGAL_دلیل_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            أرجع;
        }
        *cnvName++=c;
        وچ_Name++;
    }
    *cnvName=0;
    p_دلیلاں->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    طالما((c=*وچ_Name)!=0) {
        إذا(c==UCNV_OPTION_SEP_CHAR) {
            ++وچ_Name;
        }

        /* inName is behind an option separator */
        إذا(uprv_strncmp(وچ_Name, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            حرف *dest=pPieces->locale;

            /* copy the locale option value */
            وچ_Name+=7;
            len=0;
            طالما((c=*وچ_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++وچ_Name;

                إذا(++len>=ULOC_FULLNAME_سمرتھا) {
                    *err=U_ILLEGAL_دلیل_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    أرجع;
                }

                *dest++=c;
            }
            *dest=0;
        } وإلا إذا(uprv_strncmp(وچ_Name, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            وچ_Name+=8;
            c=*وچ_Name;
            إذا(c==0) {
                p_دلیلاں->options=(pPieces->options&=~UCNV_OPTION_نسخہ);
                أرجع;
            } وإلا إذا (static_cast<uint8_t>(c - '0') < 10) {
                p_دلیلاں->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_نسخہ) | static_cast<uint32_t>(c - '0');
                ++وچ_Name;
            }
        } وإلا إذا(uprv_strncmp(وچ_Name, "swaplfnl", 8)==0) {
            وچ_Name+=8;
            p_دلیلاں->options=(pPieces->options|=UCNV_OPTION_وٹاندرا_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } وإلا {
            /* ignore any other options until we define some */
            طالما(((c = *وچ_Name++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
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
U_CFUNC UConverter_Shared_انکڑے *
ucnv_لدو_Shared_انکڑے(ثابت حرف *converterName,
                    UConverterNamePieces *pPieces,
                    UConverter_لدو_دلیلاں *p_دلیلاں,
                    UError_کوڈ * err) {
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_لدو_دلیلاں ڈھیر_دلیلاں;
    UConverter_Shared_انکڑے *my_Shared_Converter_انکڑے = خالی_سوچک;
    UError_کوڈ اندرونی_Error_کوڈ = U_ZERO_ERROR;
    UBool mayContainOption = سچ;
    UBool checkForAlgorithmic = سچ;

    إذا (U_FAILURE (*err)) {
        أرجع خالی_سوچک;
    }

    إذا(pPieces == خالی_سوچک) {
        إذا(p_دلیلاں != خالی_سوچک) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_اندرونی_پروگرام_ERROR;
            أرجع خالی_سوچک;
        }
        pPieces = &ڈھیر_Pieces;
    }
    إذا(p_دلیلاں == خالی_سوچک) {
        uprv_memset(&ڈھیر_دلیلاں, 0, حجم(ڈھیر_دلیلاں));
        ڈھیر_دلیلاں.size = (int32_t)حجم(ڈھیر_دلیلاں);
        p_دلیلاں = &ڈھیر_دلیلاں;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    p_دلیلاں->name = converterName;
    p_دلیلاں->locale = pPieces->locale;
    p_دلیلاں->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    إذا (converterName == خالی_سوچک) {
#إذا U_CHARSET_IS_UTF8
        p_دلیلاں->name = "UTF-8";
        أرجع (UConverter_Shared_انکڑے *)converter_انکڑے[UCNV_UTF8];
#وإلا
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        p_دلیلاں->name = ucnv_getDefaultName();
        إذا (p_دلیلاں->name == خالی_سوچک) {
            *err = U_MISSING_RESOURCE_ERROR;
            أرجع خالی_سوچک;
        }
        my_Shared_Converter_انکڑے = (UConverter_Shared_انکڑے *)g_Default_Algorithmic_Shared_انکڑے;
        checkForAlgorithmic = خطأ;
        mayContainOption = g_Default_Converter_شامل_اے_Option;
        /* the default converter name is already canonical */
#جے_انت
    }
    وإلا إذا(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        p_دلیلاں->name = "UTF-8";
        أرجع (UConverter_Shared_انکڑے *)converter_انکڑے[UCNV_UTF8];
    }
    وإلا {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, p_دلیلاں, err);
        إذا (U_FAILURE(*err)) {
            /* Very bad name used. */
            أرجع خالی_سوچک;
        }

        /* get the canonical converter name */
        p_دلیلاں->name = ucnv_io_getConverterName(p_دلیلاں->name, &mayContainOption, &اندرونی_Error_کوڈ);
        إذا (U_FAILURE(اندرونی_Error_کوڈ) || p_دلیلاں->name == خالی_سوچک) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            p_دلیلاں->name = pPieces->cnvName;
        } وإلا إذا (اندرونی_Error_کوڈ == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    إذا(mayContainOption && p_دلیلاں->name != pPieces->cnvName) {
        parseConverterOptions(p_دلیلاں->name, pPieces, p_دلیلاں, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    إذا (checkForAlgorithmic) {
        my_Shared_Converter_انکڑے = (UConverter_Shared_انکڑے *)getAlgorithmicTypeFromName(p_دلیلاں->name);
    }
    إذا (my_Shared_Converter_انکڑے == خالی_سوچک)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        p_دلیلاں->nestedLoads=1;
        p_دلیلاں->pkg=خالی_سوچک;

        umtx_جندرا(&cnvCacheMutex);
        my_Shared_Converter_انکڑے = ucnv_لدو(p_دلیلاں, err);
        umtx_unlock(&cnvCacheMutex);
        إذا (U_FAILURE (*err) || (my_Shared_Converter_انکڑے == خالی_سوچک))
        {
            أرجع خالی_سوچک;
        }
    }

    أرجع my_Shared_Converter_انکڑے;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ثابت حرف* converterName,
                     UError_کوڈ* err)
{
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_لدو_دلیلاں ڈھیر_دلیلاں=UCNV_لدو_دلیلاں_INITIALIZER;
    UConverter_Shared_انکڑے *my_Shared_Converter_انکڑے;

    UTRACE_اندراج_OC(UTRACE_UCNV_OPEN);

    إذا(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_انکڑے = ucnv_لدو_Shared_انکڑے(converterName, &ڈھیر_Pieces, &ڈھیر_دلیلاں, err);

        myUConverter = ucnv_create_Converter_From_Shared_انکڑے(
            myUConverter, my_Shared_Converter_انکڑے,
            &ڈھیر_دلیلاں,
            err);

        إذا(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_حالت(myUConverter, *err);
            أرجع myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_حالت(*err);
    أرجع خالی_سوچک;
}

U_CFUNC UBool
ucnv_canCreateConverter(ثابت حرف *converterName, UError_کوڈ *err) {
    UConverter myUConverter;
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_لدو_دلیلاں ڈھیر_دلیلاں=UCNV_لدو_دلیلاں_INITIALIZER;
    UConverter_Shared_انکڑے *my_Shared_Converter_انکڑے;

    UTRACE_اندراج_OC(UTRACE_UCNV_OPEN);

    إذا(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ڈھیر_دلیلاں.onlyTestIsLoadable=سچ;
        my_Shared_Converter_انکڑے = ucnv_لدو_Shared_انکڑے(converterName, &ڈھیر_Pieces, &ڈھیر_دلیلاں, err);
        ucnv_create_Converter_From_Shared_انکڑے(
            &myUConverter, my_Shared_Converter_انکڑے,
            &ڈھیر_دلیلاں,
            err);
        ucnv_unload_Shared_انکڑے_If_Ready(my_Shared_Converter_انکڑے);
    }

    UTRACE_EXIT_حالت(*err);
    أرجع U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ثابت حرف* locale,
                                uint32_t options,
                                UError_کوڈ* err) {
    UConverter *cnv;
    ثابت UConverter_Shared_انکڑے *shared_انکڑے;
    UConverter_لدو_دلیلاں ڈھیر_دلیلاں=UCNV_لدو_دلیلاں_INITIALIZER;

    UTRACE_اندراج_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    إذا(type<0 || UCNV_نمبر_دا_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_دلیل_ERROR;
        UTRACE_EXIT_حالت(U_ILLEGAL_دلیل_ERROR);
        أرجع خالی_سوچک;
    }

    shared_انکڑے = converter_انکڑے[type];
    إذا(shared_انکڑے == خالی_سوچک || shared_انکڑے->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_دلیل_ERROR;
        UTRACE_EXIT_حالت(U_ILLEGAL_دلیل_ERROR);
        أرجع خالی_سوچک;
    }

    ڈھیر_دلیلاں.name = "";
    ڈھیر_دلیلاں.options = options;
    ڈھیر_دلیلاں.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_انکڑے(
            myUConverter, const_cast<UConverter_Shared_انکڑے*>(shared_انکڑے),
            &ڈھیر_دلیلاں, err);

    UTRACE_EXIT_PTR_حالت(cnv, *err);
    أرجع cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ثابت حرف *packageName, ثابت حرف *converterName, UError_کوڈ * err)
{
    UConverter *myUConverter;
    UConverter_Shared_انکڑے *my_Shared_Converter_انکڑے;
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_لدو_دلیلاں ڈھیر_دلیلاں=UCNV_لدو_دلیلاں_INITIALIZER;

    UTRACE_اندراج_OC(UTRACE_UCNV_OPEN_PACKAGE);

    إذا(U_FAILURE(*err)) {
        UTRACE_EXIT_حالت(*err);
        أرجع خالی_سوچک;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ڈھیر_Pieces.cnvName[0] = 0;
    ڈھیر_Pieces.locale[0] = 0;
    ڈھیر_Pieces.options = 0;
    parseConverterOptions(converterName, &ڈھیر_Pieces, &ڈھیر_دلیلاں, err);
    إذا (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_حالت(*err);
        أرجع خالی_سوچک;
    }
    ڈھیر_دلیلاں.nestedLoads=1;
    ڈھیر_دلیلاں.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_انکڑے = createConverterFromFile(&ڈھیر_دلیلاں, err);

    إذا (U_FAILURE(*err)) {
        UTRACE_EXIT_حالت(*err);
        أرجع خالی_سوچک;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_انکڑے(خالی_سوچک, my_Shared_Converter_انکڑے, &ڈھیر_دلیلاں, err);

    إذا (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_حالت(*err);
        أرجع خالی_سوچک;
    }

    UTRACE_EXIT_PTR_حالت(myUConverter, *err);
    أرجع myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_انکڑے(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_انکڑے* my_Shared_Converter_انکڑے,
                                   UConverter_لدو_دلیلاں* p_دلیلاں,
                                   UError_کوڈ* err)
{
    UBool isCopyLocal;

    إذا(U_FAILURE(*err)) {
        ucnv_unload_Shared_انکڑے_If_Ready(my_Shared_Converter_انکڑے);
        أرجع myUConverter;
    }
    إذا(myUConverter == خالی_سوچک)
    {
        myUConverter = (UConverter *) uprv_malloc (حجم (UConverter));
        إذا(myUConverter == خالی_سوچک)
        {
            *err = U_حافظہ_ونڈ_ERROR;
            ucnv_unload_Shared_انکڑے_If_Ready(my_Shared_Converter_انکڑے);
            أرجع خالی_سوچک;
        }
        isCopyLocal = خطأ;
    } وإلا {
        isCopyLocal = سچ;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, حجم(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_انکڑے = my_Shared_Converter_انکڑے;
    myUConverter->options = p_دلیلاں->options;
    إذا(!p_دلیلاں->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_منزل_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->منزل_Unicode_حالت = my_Shared_Converter_انکڑے->منزل_Unicode_حالت;
        myUConverter->max_بائٹاں_فی_UChar = my_Shared_Converter_انکڑے->static_انکڑے->max_بائٹاں_فی_Char;
        myUConverter->subChar1 = my_Shared_Converter_انکڑے->static_انکڑے->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_انکڑے->static_انکڑے->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_انکڑے->static_انکڑے->subChar, myUConverter->subCharLen);
        myUConverter->منزل_UCallback_Reason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    إذا(my_Shared_Converter_انکڑے->impl->open != خالی_سوچک) {
        my_Shared_Converter_انکڑے->impl->open(myUConverter, p_دلیلاں, err);
        إذا(U_FAILURE(*err) && !p_دلیلاں->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            أرجع خالی_سوچک;
        }
    }

    أرجع myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_انکڑے *my_Shared_انکڑے = خالی_سوچک;
    int32_t pos;
    int32_t جدول_Deleted_عدد = 0;
    ثابت UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, بچیا;

    UTRACE_اندراج_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    إذا (SHARED_انکڑے_HASHTABLE == خالی_سوچک) {
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
    umtx_جندرا(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    افعل {
        بچیا = 0;
        pos = UHASH_پہلا;
        طالما ((e = uhash_اگلا_Element (SHARED_انکڑے_HASHTABLE, &pos)) != خالی_سوچک)
        {
            my_Shared_انکڑے = (UConverter_Shared_انکڑے *) e->value.اشارہ_گر;
            /*deletes only if reference counter == 0 */
            إذا (my_Shared_انکڑے->referenceCounter == 0)
            {
                جدول_Deleted_عدد++;

                UCNV_DEBUG_LOG("del",my_Shared_انکڑے->static_انکڑے->name,my_Shared_انکڑے);

                uhash_removeElement(SHARED_انکڑے_HASHTABLE, e);
                my_Shared_انکڑے->shared_انکڑے_Cached = خطأ;
                ucnv_delete_Shared_Converter_انکڑے (my_Shared_انکڑے);
            } وإلا {
                ++بچیا;
            }
        }
    } طالما(++i == 1 && بچیا > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", بچیا);

    UTRACE_EXIT_قدر(جدول_Deleted_عدد);
    أرجع جدول_Deleted_عدد;
}

/* available converters list --------------------------------------------------- */

ساكن فراغ U_CALLCONV init_دستیاب_Converters_List(UError_کوڈ &err_کوڈ) {
    U_ASSERT(g_دستیاب_Converter_گنتی == 0);
    U_ASSERT(g_دستیاب_Converters == خالی_سوچک);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_open_All_ناں(&err_کوڈ);
    int32_t all_Converter_گنتی = uenum_گنتی(allConvEnum, &err_کوڈ);
    إذا (U_FAILURE(err_کوڈ)) {
        أرجع;
    }

    /* We can't have more than "*converterTable" converters to open */
    g_دستیاب_Converters = static_cast<ثابت حرف**>(uprv_malloc(all_Converter_گنتی * حجم(حرف*)));
    إذا (!g_دستیاب_Converters) {
        err_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UError_کوڈ local_حالت = U_ZERO_ERROR;
    UConverter عارضی_Converter;
    ucnv_close(ucnv_createConverter(&عارضی_Converter, خالی_سوچک, &local_حالت));

    g_دستیاب_Converter_گنتی = 0;

    لكل (int32_t idx = 0; idx < all_Converter_گنتی; idx++) {
        local_حالت = U_ZERO_ERROR;
        ثابت حرف *converterName = uenum_اگلا(allConvEnum, خالی_سوچک, &local_حالت);
        إذا (ucnv_canCreateConverter(converterName, &local_حالت)) {
            g_دستیاب_Converters[g_دستیاب_Converter_گنتی++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


ساكن UBool have_دستیاب_Converter_List(UError_کوڈ *p_Error_کوڈ) {
    umtx_initOnce(g_دستیاب_Converters_Init_Once, &init_دستیاب_Converters_List, *p_Error_کوڈ);
    أرجع U_SUCCESS(*p_Error_کوڈ);
}

U_CFUNC uint16_t
ucnv_bld_گنتی_دستیاب_Converters(UError_کوڈ *p_Error_کوڈ) {
    إذا (have_دستیاب_Converter_List(p_Error_کوڈ)) {
        أرجع g_دستیاب_Converter_گنتی;
    }
    أرجع 0;
}

U_CFUNC ثابت حرف *
ucnv_bld_get_دستیاب_Converter(uint16_t n, UError_کوڈ *p_Error_کوڈ) {
    إذا (have_دستیاب_Converter_List(p_Error_کوڈ)) {
        إذا (n < g_دستیاب_Converter_گنتی) {
            أرجع g_دستیاب_Converters[n];
        }
        *p_Error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
    }
    أرجع خالی_سوچک;
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
اندرونی_سیٹ_Name(ثابت حرف *name, UError_کوڈ *حالت) {
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_لدو_دلیلاں ڈھیر_دلیلاں=UCNV_لدو_دلیلاں_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool شامل_اے_Option = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != خالی_سوچک);
    ثابت UConverter_Shared_انکڑے *algorithmic_Shared_انکڑے;

    ڈھیر_دلیلاں.name = name;
    إذا(شامل_اے_Option) {
        ڈھیر_Pieces.cnvName[0] = 0;
        ڈھیر_Pieces.locale[0] = 0;
        ڈھیر_Pieces.options = 0;
        parseConverterOptions(name, &ڈھیر_Pieces, &ڈھیر_دلیلاں, حالت);
        إذا(U_FAILURE(*حالت)) {
            أرجع;
        }
    }
    algorithmic_Shared_انکڑے = getAlgorithmicTypeFromName(ڈھیر_دلیلاں.name);

    umtx_جندرا(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_انکڑے = algorithmic_Shared_انکڑے;
    g_Default_Converter_شامل_اے_Option = شامل_اے_Option;
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
#جے_انت

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
        icu::Mutex جندرا(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    إذا(name==خالی_سوچک) {
        UError_کوڈ error_کوڈ = U_ZERO_ERROR;
        UConverter *cnv = خالی_سوچک;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        إذا(name != خالی_سوچک) {
            cnv = ucnv_open(name, &error_کوڈ);
            إذا(U_SUCCESS(error_کوڈ) && cnv != خالی_سوچک) {
                name = ucnv_getName(cnv, &error_کوڈ);
            }
        }

        إذا(name == خالی_سوچک || name[0] == 0
            || U_FAILURE(error_کوڈ) || cnv == خالی_سوچک
            || uprv_strlen(name)>=حجم(g_Default_Converter_Name_بفر))
        {
            /* Panic time, let's use a fallback. */
#إذا (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#وإلاإذا U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_وٹاندرا_LFNL_OPTION_STRING;
#وإلا
            name = "ibm-37_P100-1995";
#جے_انت
        }

        اندرونی_سیٹ_Name(name, &error_کوڈ);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    أرجع name;
#جے_انت
}

#إذا U_CHARSET_IS_UTF8
U_CAPI فراغ U_EXPORT2 ucnv_سیٹ_Default_Name(ثابت حرف *) {}
#وإلا
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI فراغ U_EXPORT2
ucnv_سیٹ_Default_Name(ثابت حرف *converterName) {
    إذا(converterName==خالی_سوچک) {
        /* reset to the default codepage */
        gDefaultConverterName=خالی_سوچک;
    } وإلا {
        UError_کوڈ error_کوڈ = U_ZERO_ERROR;
        UConverter *cnv = خالی_سوچک;
        ثابت حرف *name = خالی_سوچک;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &error_کوڈ);
        إذا(U_SUCCESS(error_کوڈ) && cnv != خالی_سوچک) {
            name = ucnv_getName(cnv, &error_کوڈ);
        }

        إذا(U_SUCCESS(error_کوڈ) && name!=خالی_سوچک) {
            اندرونی_سیٹ_Name(name, &error_کوڈ);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#جے_انت

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#إذا !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_وٹاندرا(ثابت UDataSwapper *ds,
          ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
          UError_کوڈ *p_Error_کوڈ) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_ناپ;

    ثابت uint8_t *وچ_بائٹاں;
    uint8_t *باہرلا_بائٹاں;

    uint32_t وتھ, count, static_انکڑے_ناپ;
    int32_t size;

    ثابت UConverter_Static_انکڑے *وچ_Static_انکڑے;
    UConverter_Static_انکڑے *باہرلا_Static_انکڑے;

    ثابت _MBCSHeader *وچ_MBCSHeader;
    _MBCSHeader *باہرلا_MBCSHeader;
    _MBCSHeader mbcs_سرخی;
    uint32_t mbcs_سرخی_لمائی;
    UBool noFromU=خطأ;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_اشاریہ_لمائی;

    ثابت int32_t *وچ_Ext_Indexes;
    int32_t ext_وتھ;

    /* udata_swapDataHeader checks the arguments */
    سرخی_ناپ=udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے+4);
    إذا(!(
        pInfo->انکڑے_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->انکڑے_Format[1]==0x6e &&
        pInfo->انکڑے_Format[2]==0x76 &&
        pInfo->انکڑے_Format[3]==0x74 &&
        pInfo->format_نسخہ[0]==6 &&
        pInfo->format_نسخہ[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->انکڑے_Format[0], pInfo->انکڑے_Format[1],
                         pInfo->انکڑے_Format[2], pInfo->انکڑے_Format[3],
                         pInfo->format_نسخہ[0], pInfo->format_نسخہ[1]);
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    وچ_بائٹاں=(ثابت uint8_t *)وچ_انکڑے+سرخی_ناپ;
    باہرلا_بائٹاں=(باہرلا_انکڑے == خالی_سوچک) ? خالی_سوچک : (uint8_t *)باہرلا_انکڑے+سرخی_ناپ;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    وچ_Static_انکڑے=(ثابت UConverter_Static_انکڑے *)وچ_بائٹاں;
    باہرلا_Static_انکڑے=(UConverter_Static_انکڑے *)باہرلا_بائٹاں;

    إذا(length<0) {
        static_انکڑے_ناپ=ds->readUInt32(وچ_Static_انکڑے->struct_ناپ);
    } وإلا {
        length-=سرخی_ناپ;
        إذا( length<(int32_t)حجم(UConverter_Static_انکڑے) ||
            (uint32_t)length<(static_انکڑے_ناپ=ds->readUInt32(وچ_Static_انکڑے->struct_ناپ))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    إذا(length>=0) {
        /* swap the static data */
        إذا(وچ_Static_انکڑے!=باہرلا_Static_انکڑے) {
            uprv_memcpy(باہرلا_Static_انکڑے, وچ_Static_انکڑے, static_انکڑے_ناپ);
        }

        ds->وٹاندرا_Array32(ds, &وچ_Static_انکڑے->struct_ناپ, 4,
                           &باہرلا_Static_انکڑے->struct_ناپ, p_Error_کوڈ);
        ds->وٹاندرا_Array32(ds, &وچ_Static_انکڑے->codepage, 4,
                           &باہرلا_Static_انکڑے->codepage, p_Error_کوڈ);

        ds->وٹاندرا_Inv_Chars(ds, وچ_Static_انکڑے->name, (int32_t)uprv_strlen(وچ_Static_انکڑے->name),
                            باہرلا_Static_انکڑے->name, p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            أرجع 0;
        }
    }

    وچ_بائٹاں+=static_انکڑے_ناپ;
    إذا (باہرلا_بائٹاں != خالی_سوچک) باہرلا_بائٹاں+=static_انکڑے_ناپ;
    إذا(length>=0) {
        length-=(int32_t)static_انکڑے_ناپ;
    }

    /* check for supported conversionType values */
    إذا(وچ_Static_انکڑے->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        وچ_MBCSHeader=(ثابت _MBCSHeader *)وچ_بائٹاں;
        باہرلا_MBCSHeader=(_MBCSHeader *)باہرلا_بائٹاں;

        إذا(0<=length && length<(int32_t)حجم(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        إذا(وچ_MBCSHeader->نسخہ[0]==4 && وچ_MBCSHeader->نسخہ[1]>=1) {
            mbcs_سرخی_لمائی=MBCS_سرخی_V4_لمائی;
        } وإلا إذا(وچ_MBCSHeader->نسخہ[0]==5 && وچ_MBCSHeader->نسخہ[1]>=3 &&
                  ((mbcs_سرخی.options=ds->readUInt32(وچ_MBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_نقاب)==0
        ) {
            mbcs_سرخی_لمائی=mbcs_سرخی.options&MBCS_OPT_لمائی_نقاب;
            noFromU = (mbcs_سرخی.options & MBCS_OPT_NO_FROM_U) != 0;
        } وإلا {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             وچ_MBCSHeader->نسخہ[0], وچ_MBCSHeader->نسخہ[1]);
            *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        uprv_memcpy(mbcs_سرخی.نسخہ, وچ_MBCSHeader->نسخہ, 4);
        mbcs_سرخی.گنتی_حالتاں=         ds->readUInt32(وچ_MBCSHeader->گنتی_حالتاں);
        mbcs_سرخی.گنتی_منزل_UFallbacks=   ds->readUInt32(وچ_MBCSHeader->گنتی_منزل_UFallbacks);
        mbcs_سرخی.وتھ_منزل_UCode_Units=  ds->readUInt32(وچ_MBCSHeader->وتھ_منزل_UCode_Units);
        mbcs_سرخی.وتھ_From_UTable=    ds->readUInt32(وچ_MBCSHeader->وتھ_From_UTable);
        mbcs_سرخی.وتھ_From_UBytes=    ds->readUInt32(وچ_MBCSHeader->وتھ_From_UBytes);
        mbcs_سرخی.flags=               ds->readUInt32(وچ_MBCSHeader->flags);
        mbcs_سرخی.from_UBytes_لمائی=    ds->readUInt32(وچ_MBCSHeader->from_UBytes_لمائی);
        /* mbcsHeader.options have been read above */

        ext_وتھ=(int32_t)(mbcs_سرخی.flags>>8);
        outputType=(uint8_t)mbcs_سرخی.flags;
        إذا(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
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
            *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
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
        mbcs_اشاریہ_لمائی=0;
        إذا( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcs_سرخی.نسخہ[1]>=3 && (maxFastUChar=mbcs_سرخی.نسخہ[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_اشاریہ_لمائی=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        إذا(ext_وتھ==0) {
            size=(int32_t)(mbcs_سرخی.وتھ_From_UBytes+mbcs_اشاریہ_لمائی);
            إذا(!noFromU) {
                size+=(int32_t)mbcs_سرخی.from_UBytes_لمائی;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            وچ_Ext_Indexes=خالی_سوچک;
        } وإلا {
            /* there is extension data after the base data, see ucnv_ext.h */
            إذا(length>=0 && length<(ext_وتھ+UCNV_EXT_INDEXES_MIN_لمائی*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            وچ_Ext_Indexes=(ثابت int32_t *)(وچ_بائٹاں+ext_وتھ);
            size=ext_وتھ+udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_ناپ]);
        }

        إذا(length>=0) {
            إذا(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            /* copy the data for inaccessible bytes */
            إذا(وچ_بائٹاں!=باہرلا_بائٹاں) {
                uprv_memcpy(باہرلا_بائٹاں, وچ_بائٹاں, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_سرخی_لمائی*4;
            ds->وٹاندرا_Array32(ds, &وچ_MBCSHeader->گنتی_حالتاں, count-4,
                               &باہرلا_MBCSHeader->گنتی_حالتاں, p_Error_کوڈ);

            إذا(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ثابت حرف *وچ_ادھار_Name=(ثابت حرف *)وچ_بائٹاں+count;
                حرف *باہرلا_ادھار_Name=(حرف *)باہرلا_بائٹاں+count;
                ds->وٹاندرا_Inv_Chars(ds, وچ_ادھار_Name, (int32_t)uprv_strlen(وچ_ادھار_Name),
                                    باہرلا_ادھار_Name, p_Error_کوڈ);
            } وإلا {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                وتھ=count;
                count=mbcs_سرخی.گنتی_حالتاں*1024;
                ds->وٹاندرا_Array32(ds, وچ_بائٹاں+وتھ, (int32_t)count,
                                   باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* swap the toUFallbacks[] */
                وتھ+=count;
                count=mbcs_سرخی.گنتی_منزل_UFallbacks*8;
                ds->وٹاندرا_Array32(ds, وچ_بائٹاں+وتھ, (int32_t)count,
                                   باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* swap the unicodeCodeUnits[] */
                وتھ=mbcs_سرخی.وتھ_منزل_UCode_Units;
                count=mbcs_سرخی.وتھ_From_UTable-وتھ;
                ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, (int32_t)count,
                                   باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* offset to the stage 1 table, independent of the outputType */
                وتھ=mbcs_سرخی.وتھ_From_UTable;

                إذا(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcs_سرخی.وتھ_From_UBytes-وتھ)+mbcs_سرخی.from_UBytes_لمائی;
                    ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, (int32_t)count,
                                       باہرلا_بائٹاں+وتھ, p_Error_کوڈ);
                } وإلا {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    إذا(وچ_Static_انکڑے->unicode_نقاب&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } وإلا {
                        count=0x40*2; /* only BMP */
                    }
                    ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, (int32_t)count,
                                       باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                    /* stage 2 table: uint32_t[] */
                    وتھ+=count;
                    count=mbcs_سرخی.وتھ_From_UBytes-وتھ;
                    ds->وٹاندرا_Array32(ds, وچ_بائٹاں+وتھ, (int32_t)count,
                                       باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    وتھ=mbcs_سرخی.وتھ_From_UBytes;
                    count= noFromU ? 0 : mbcs_سرخی.from_UBytes_لمائی;
                    بدل(outputType) {
                    حالة MBCS_OUTPUT_2:
                    حالة MBCS_OUTPUT_3_EUC:
                    حالة MBCS_OUTPUT_2_SISO:
                        ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, (int32_t)count,
                                           باہرلا_بائٹاں+وتھ, p_Error_کوڈ);
                        اكسر;
                    حالة MBCS_OUTPUT_4:
                        ds->وٹاندرا_Array32(ds, وچ_بائٹاں+وتھ, (int32_t)count,
                                           باہرلا_بائٹاں+وتھ, p_Error_کوڈ);
                        اكسر;
                    افتراضي:
                        /* just uint8_t[], nothing to swap */
                        اكسر;
                    }

                    إذا(mbcs_اشاریہ_لمائی!=0) {
                        وتھ+=count;
                        count=mbcs_اشاریہ_لمائی;
                        ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, (int32_t)count,
                                           باہرلا_بائٹاں+وتھ, p_Error_کوڈ);
                    }
                }
            }

            إذا(ext_وتھ!=0) {
                /* swap the extension data */
                وچ_بائٹاں+=ext_وتھ;
                باہرلا_بائٹاں+=ext_وتھ;

                /* swap toUTable[] */
                وتھ=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_منزل_U_اشاریہ]);
                length=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_منزل_U_لمائی]);
                ds->وٹاندرا_Array32(ds, وچ_بائٹاں+وتھ, length*4, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* swap toUUChars[] */
                وتھ=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_منزل_U_UCHARS_اشاریہ]);
                length=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_منزل_U_UCHARS_لمائی]);
                ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, length*2, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* swap fromUTableUChars[] */
                وتھ=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_FROM_U_UCHARS_اشاریہ]);
                length=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_FROM_U_لمائی]);
                ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, length*2, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* swap fromUTableValues[] */
                وتھ=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_FROM_U_اقدار_اشاریہ]);
                /* same length as for fromUTableUChars[] */
                ds->وٹاندرا_Array32(ds, وچ_بائٹاں+وتھ, length*4, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                وتھ=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_اشاریہ]);
                length=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_لمائی]);
                ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, length*2, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* swap fromUStage3[] */
                وتھ=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_اشاریہ]);
                length=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_لمائی]);
                ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, length*2, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* swap fromUStage3b[] */
                وتھ=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_اشاریہ]);
                length=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_لمائی]);
                ds->وٹاندرا_Array32(ds, وچ_بائٹاں+وتھ, length*4, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);

                /* swap indexes[] */
                length=udata_readInt32(ds, وچ_Ext_Indexes[UCNV_EXT_INDEXES_لمائی]);
                ds->وٹاندرا_Array32(ds, وچ_بائٹاں, length*4, باہرلا_بائٹاں, p_Error_کوڈ);
            }
        }
    } وإلا {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         وچ_Static_انکڑے->conversionType);
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    أرجع سرخی_ناپ+(int32_t)static_انکڑے_ناپ+size;
}

#جے_انت /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#جے_انت
