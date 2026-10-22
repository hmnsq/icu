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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/putil.h"
#شامل "unicode/udata.h"
#شامل "unicode/ucnv.h"
#شامل "unicode/uloc.h"
#شامل "mutex.h"
#شامل "putilimp.h"
#شامل "uassert.h"
#شامل "utracimp.h"
#شامل "ucnv_io.h"
#شامل "ucnv_bld.h"
#شامل "ucnvmbcs.h"
#شامل "ucnv_ext.h"
#شامل "ucnv_cnv.h"
#شامل "ucnv_imp.h"
#شامل "uhash.h"
#شامل "umutex.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "ucln_cmn.h"
#شامل "ustr_cnv.h"


#اگر 0
#شامل <stdio.h>
خارجی خالی UCNV_DEBUG_LOG(حرف *what, حرف *who, خالی *p, صحیح l);
#تعریف_کرو UCNV_DEBUG_LOG(x,y,z) UCNV_DEBUG_LOG(x,y,z,__LINE__)
#ورنہ
# define UCNV_DEBUG_LOG(x,y,z)
#اگر_ختم

ساکن ثابت UConverter_Shared_ڈیٹا * ثابت
converter_ڈیٹا[UCNV_نمبر_کا_SUPPORTED_CONVERTER_TYPES]={
    خالی_اشاریہ, خالی_اشاریہ,

#اگر UCONFIG_NO_LEGACY_CONVERSION
    خالی_اشاریہ,
#ورنہ
    &_MBCSData,
#اگر_ختم

    &Latin1_ڈیٹا,
    &UTF8_ڈیٹا, &_UTF16BEData, &_UTF16LEData,
#اگر UCONFIG_ONLY_HTML_CONVERSION
    خالی_اشاریہ, خالی_اشاریہ,
#ورنہ
    &_UTF32BEData, &_UTF32LEData,
#اگر_ختم
    خالی_اشاریہ,

#اگر UCONFIG_NO_LEGACY_CONVERSION
    خالی_اشاریہ,
#ورنہ
    &ISO2022_ڈیٹا,
#اگر_ختم

#اگر UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ,
    خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ,
    خالی_اشاریہ,
#ورنہ
    &_LMBCSData1,&_LMBCSData2, &_LMBCSData3, &_LMBCSData4, &_LMBCSData5, &_LMBCSData6,
    &_LMBCSData8,&_LMBCSData11,&_LMBCSData16,&_LMBCSData17,&_LMBCSData18,&_LMBCSData19,
    &_HZData,
#اگر_ختم

#اگر UCONFIG_ONLY_HTML_CONVERSION
    خالی_اشاریہ,
#ورنہ
    &_SCSUData,
#اگر_ختم


#اگر UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    خالی_اشاریہ,
#ورنہ
    &_ISCIIData,
#اگر_ختم

    &_ASCIIData,
#اگر UCONFIG_ONLY_HTML_CONVERSION
    خالی_اشاریہ, خالی_اشاریہ, &UTF16_ڈیٹا, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ,
#ورنہ
    &UTF7_ڈیٹا, &Bocu1_ڈیٹا, &UTF16_ڈیٹا, &UTF32_ڈیٹا, &CESU8_ڈیٹا, &_IMAPData,
#اگر_ختم

#اگر UCONFIG_NO_LEGACY_CONVERSION || UCONFIG_ONLY_HTML_CONVERSION
    خالی_اشاریہ,
#ورنہ
    &Compound_متن_ڈیٹا
#اگر_ختم
};

/* Please keep this in binary sorted order for getAlgorithmicTypeFromName.
   Also the name should be in lower case and all spaces, dashes and underscores
   removed
*/
ساکن ساخت {
  ثابت حرف *name;
  ثابت UConverterType type;
} ثابت cnvNameType[] = {
#اگر !UCONFIG_ONLY_HTML_CONVERSION
  { "bocu1", UCNV_BOCU1 },
  { "cesu8", UCNV_CESU8 },
#اگر_ختم
#اگر !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "hz",UCNV_HZ },
#اگر_ختم
#اگر !UCONFIG_ONLY_HTML_CONVERSION
  { "imapmailboxname", UCNV_IMAP_MAILBOX },
#اگر_ختم
#اگر !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
  { "iscii", UCNV_ISCII },
#اگر_ختم
#اگر !UCONFIG_NO_LEGACY_CONVERSION
  { "iso2022", UCNV_ISO_2022 },
#اگر_ختم
  { "iso88591", UCNV_LATIN_1 },
#اگر !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION
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
#اگر_ختم
#اگر !UCONFIG_ONLY_HTML_CONVERSION
  { "scsu", UCNV_SCSU },
#اگر_ختم
  { "usascii", UCNV_US_ASCII },
  { "utf16", UCNV_UTF16 },
  { "utf16be", UCNV_UTF16_BigEndian },
  { "utf16le", UCNV_UTF16_LittleEndian },
#اگر U_IS_BIG_ENDIAN
  { "utf16oppositeendian", UCNV_UTF16_LittleEndian },
  { "utf16platformendian", UCNV_UTF16_BigEndian },
#ورنہ
  { "utf16oppositeendian", UCNV_UTF16_BigEndian},
  { "utf16platformendian", UCNV_UTF16_LittleEndian },
#اگر_ختم
#اگر !UCONFIG_ONLY_HTML_CONVERSION
  { "utf32", UCNV_UTF32 },
  { "utf32be", UCNV_UTF32_BigEndian },
  { "utf32le", UCNV_UTF32_LittleEndian },
#اگر U_IS_BIG_ENDIAN
  { "utf32oppositeendian", UCNV_UTF32_LittleEndian },
  { "utf32platformendian", UCNV_UTF32_BigEndian },
#ورنہ
  { "utf32oppositeendian", UCNV_UTF32_BigEndian },
  { "utf32platformendian", UCNV_UTF32_LittleEndian },
#اگر_ختم
#اگر_ختم
#اگر !UCONFIG_ONLY_HTML_CONVERSION
  { "utf7", UCNV_UTF7 },
#اگر_ختم
  { "utf8", UCNV_UTF8 },
#اگر !UCONFIG_ONLY_HTML_CONVERSION
  { "x11compoundtext", UCNV_COMPOUND_متن}
#اگر_ختم
};


/*initializes some global variables */
ساکن UHashtable *SHARED_ڈیٹا_HASHTABLE = خالی_اشاریہ;
ساکن icu::UMutex cnvCacheMutex;
/*  Note:  the global mutex is used for      */
/*         reference count updates.          */

ساکن ثابت حرف **g_دستیاب_Converters = خالی_اشاریہ;
ساکن uint16_t g_دستیاب_Converter_گنتی = 0;
ساکن icu::UInitOnce g_دستیاب_Converters_Init_Once {};

#اگر !U_CHARSET_IS_UTF8

/* This contains the resolved converter name. So no further alias lookup is needed again. */
ساکن حرف g_Default_Converter_Name_بفر[UCNV_MAX_CONVERTER_NAME_لمبائی + 1]; /* +1 for nullptr */
ساکن ثابت حرف *gDefaultConverterName = خالی_اشاریہ;

/*
If the default converter is an algorithmic converter, this is the cached value.
We don't cache a full UConverter and clone it because ucnv_clone doesn't have
less overhead than an algorithmic open. We don't cache non-algorithmic converters
because ucnv_flushCache must be able to unload the default converter and its table.
*/
ساکن ثابت UConverter_Shared_ڈیٹا *g_Default_Algorithmic_Shared_ڈیٹا = خالی_اشاریہ;

/* Does gDefaultConverterName have a converter option and require extra parsing? */
ساکن UBool g_Default_Converter_شامل_ہے_Option;

#اگر_ختم  /* !U_CHARSET_IS_UTF8 */

ساکن ثابت حرف ڈیٹا_TYPE[] = "cnv";

/* ucnv_flushAvailableConverterCache. This is only called from ucnv_cleanup().
 *                       If it is ever to be called from elsewhere, synchronization
 *                       will need to be considered.
 */
ساکن خالی
ucnv_flush_دستیاب_Converter_Cache() {
    g_دستیاب_Converter_گنتی = 0;
    اگر (g_دستیاب_Converters) {
        uprv_free(const_cast<حرف**>(g_دستیاب_Converters));
        g_دستیاب_Converters = خالی_اشاریہ;
    }
    g_دستیاب_Converters_Init_Once.reset();
}

/* ucnv_cleanup - delete all storage held by the converter cache, except any  */
/*                in use by open converters.                                  */
/*                Not thread safe.                                            */
/*                Not supported API.                                          */
ساکن UBool U_CALLCONV ucnv_cleanup() {
    ucnv_flushCache();
    اگر (SHARED_ڈیٹا_HASHTABLE != خالی_اشاریہ && uhash_گنتی(SHARED_ڈیٹا_HASHTABLE) == 0) {
        uhash_close(SHARED_ڈیٹا_HASHTABLE);
        SHARED_ڈیٹا_HASHTABLE = خالی_اشاریہ;
    }

    /* Isn't called from flushCache because other threads may have preexisting references to the table. */
    ucnv_flush_دستیاب_Converter_Cache();

#اگر !U_CHARSET_IS_UTF8
    gDefaultConverterName = خالی_اشاریہ;
    g_Default_Converter_Name_بفر[0] = 0;
    g_Default_Converter_شامل_ہے_Option = جھوٹ;
    g_Default_Algorithmic_Shared_ڈیٹا = خالی_اشاریہ;
#اگر_ختم

    واپس (SHARED_ڈیٹا_HASHTABLE == خالی_اشاریہ);
}

U_CAPI خالی U_EXPORT2
ucnv_enableCleanup() {
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UCNV, ucnv_cleanup);
}

ساکن UBool U_CALLCONV
isCnvAcceptable(خالی * /*context*/,
                ثابت حرف * /*type*/, ثابت حرف * /*name*/,
                ثابت UDataInfo *pInfo) {
    واپس
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->ڈیٹا_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->ڈیٹا_Format[1]==0x6e &&
        pInfo->ڈیٹا_Format[2]==0x76 &&
        pInfo->ڈیٹا_Format[3]==0x74 &&
        pInfo->format_نسخہ[0]==6;  /* Everything will be version 6 */
}

/**
 * Un flatten shared data from a UDATA..
 */
ساکن UConverter_Shared_ڈیٹا*
ucnv_ڈیٹا_un_Flatten_Clone(UConverter_بھرو_دلائل *p_دلائل, UData_حافظہ *p_ڈیٹا, UError_رمز *حالت_2)
{
    /* UDataInfo info; -- necessary only if some converters have different formatVersion */
    ثابت uint8_t* raw = static_cast<ثابت uint8_t*>(udata_get_حافظہ(p_ڈیٹا));
    ثابت UConverter_Static_ڈیٹا* ماخذ = reinterpret_cast<ثابت UConverter_Static_ڈیٹا*>(raw);
    UConverter_Shared_ڈیٹا *ڈیٹا;
    UConverterType type = static_cast<UConverterType>(ماخذ->conversionType);

    اگر(U_FAILURE(*حالت_2))
        واپس خالی_اشاریہ;

    اگر (static_cast<uint16_t>(type) >= UCNV_نمبر_کا_SUPPORTED_CONVERTER_TYPES ||
        converter_ڈیٹا[type] == خالی_اشاریہ ||
        !converter_ڈیٹا[type]->isReferenceCounted ||
        converter_ڈیٹا[type]->reference_شمار != 1 ||
        ماخذ->struct_سائز != حجم(UConverter_Static_ڈیٹا))
    {
        *حالت_2 = U_INVALID_جدول_FORMAT;
        واپس خالی_اشاریہ;
    }

    ڈیٹا = static_cast<UConverter_Shared_ڈیٹا*>(uprv_malloc(حجم(UConverter_Shared_ڈیٹا)));
    اگر(ڈیٹا == خالی_اشاریہ) {
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }

    /* copy initial values from the static structure for this type */
    uprv_memcpy(ڈیٹا, converter_ڈیٹا[type], حجم(UConverter_Shared_ڈیٹا));

    ڈیٹا->static_ڈیٹا = ماخذ;

    ڈیٹا->shared_ڈیٹا_Cached = جھوٹ;

    /* fill in fields from the loaded data */
    ڈیٹا->ڈیٹا_حافظہ = (خالی*)p_ڈیٹا; /* for future use */

    اگر(ڈیٹا->impl->load != خالی_اشاریہ) {
        ڈیٹا->impl->load(ڈیٹا, p_دلائل, raw + ماخذ->struct_سائز, حالت_2);
        اگر(U_FAILURE(*حالت_2)) {
            uprv_free(ڈیٹا);
            واپس خالی_اشاریہ;
        }
    }
    واپس ڈیٹا;
}

/*Takes an alias name gets an actual converter file name
 *goes to disk and opens it.
 *allocates the memory and returns a new UConverter object
 */
ساکن UConverter_Shared_ڈیٹا *createConverterFromFile(UConverter_بھرو_دلائل *p_دلائل, UError_رمز * err)
{
    UData_حافظہ *ڈیٹا;
    UConverter_Shared_ڈیٹا *shared_ڈیٹا;

    UTRACE_اندراج_OC(UTRACE_UCNV_بھرو);

    اگر (U_FAILURE (*err)) {
        UTRACE_EXIT_حالت(*err);
        واپس خالی_اشاریہ;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "load converter %s from package %s", p_دلائل->name, p_دلائل->pkg);

    ڈیٹا = udata_openChoice(p_دلائل->pkg, ڈیٹا_TYPE, p_دلائل->name, isCnvAcceptable, خالی_اشاریہ, err);
    اگر(U_FAILURE(*err))
    {
        UTRACE_EXIT_حالت(*err);
        واپس خالی_اشاریہ;
    }

    shared_ڈیٹا = ucnv_ڈیٹا_un_Flatten_Clone(p_دلائل, ڈیٹا, err);
    اگر(U_FAILURE(*err))
    {
        udata_close(ڈیٹا);
        UTRACE_EXIT_حالت(*err);
        واپس خالی_اشاریہ;
    }

    /*
     * TODO Store pkg in a field in the shared data so that delta-only converters
     * can load base converters from the same package.
     * If the pkg name is longer than the field, then either do not load the converter
     * in the first place, or just set the pkg field to "".
     */

    UTRACE_EXIT_PTR_حالت(shared_ڈیٹا, *err);
    واپس shared_ڈیٹا;
}

/*returns a converter type from a string
 */
ساکن ثابت UConverter_Shared_ڈیٹا *
getAlgorithmicTypeFromName(ثابت حرف *realName)
{
    uint32_t mid, آغاز, حد_بندی;
    uint32_t آخری_Mid;
    صحیح نتیجہ;
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_لمبائی];

    /* Lower case and remove ignoreable characters. */
    ucnv_io_stripForCompare(strippedName, realName);

    /* do a binary search for the alias */
    آغاز = 0;
    حد_بندی = UPRV_LENGTHOF(cnvNameType);
    mid = حد_بندی;
    آخری_Mid = UINT32_MAX;

    برائے (;;) {
        mid = (آغاز + حد_بندی) / 2;
        اگر (آخری_Mid == mid) {   /* Have we moved? */
            توڑو;  /* We haven't moved, and it wasn't found. */
        }
        آخری_Mid = mid;
        نتیجہ = uprv_strcmp(strippedName, cnvNameType[mid].name);

        اگر (نتیجہ < 0) {
            حد_بندی = mid;
        } ورنہ اگر (نتیجہ > 0) {
            آغاز = mid;
        } ورنہ {
            واپس converter_ڈیٹا[cnvNameType[mid].type];
        }
    }

    واپس خالی_اشاریہ;
}

/*
* Based on the number of known converters, this determines how many times larger
* the shared data hash table should be. When on small platforms, or just a couple
* of converters are used, this number should be 2. When memory is plentiful, or
* when ucnv_countAvailable is ever used with a lot of available converters,
* this should be 4.
* Larger numbers reduce the number of hash collisions, but use more memory.
*/
#تعریف_کرو UCNV_CACHE_بھرو_ضربی_عدد 2

/* Puts the shared data in the static hashtable SHARED_DATA_HASHTABLE */
/*   Will always be called with the cnvCacheMutex already being held   */
/*     by the calling function.                                       */
/* Stores the shared data in the SHARED_DATA_HASHTABLE
 * @param data The shared data
 */
ساکن خالی
ucnv_share_Converter_ڈیٹا(UConverter_Shared_ڈیٹا * ڈیٹا)
{
    UError_رمز err = U_ZERO_ERROR;
    /*Lazy evaluates the Hashtable itself */
    /*void *sanity = nullptr;*/

    اگر (SHARED_ڈیٹا_HASHTABLE == خالی_اشاریہ)
    {
        SHARED_ڈیٹا_HASHTABLE = uhash_open_سائز(uhash_hashChars, uhash_compareChars, خالی_اشاریہ,
                            ucnv_io_گنتی_معلوم_Converters(&err)*UCNV_CACHE_بھرو_ضربی_عدد,
                            &err);
        ucnv_enableCleanup();

        اگر (U_FAILURE(err))
            واپس;
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
    ڈیٹا->shared_ڈیٹا_Cached = سچ;

    uhash_put(SHARED_ڈیٹا_HASHTABLE,
            (خالی*) ڈیٹا->static_ڈیٹا->name, /* Okay to cast away const as long as
            keyDeleter == nullptr */
            ڈیٹا,
            &err);
    UCNV_DEBUG_LOG("put", ڈیٹا->static_ڈیٹا->name,ڈیٹا);

}

/*  Look up a converter name in the shared data cache.                    */
/*    cnvCacheMutex must be held by the caller to protect the hash table. */
/* gets the shared data from the SHARED_DATA_HASHTABLE (might return nullptr if it isn't there)
 * @param name The name of the shared data
 * @return the shared data from the SHARED_DATA_HASHTABLE
 */
ساکن UConverter_Shared_ڈیٹا *
ucnv_get_Shared_Converter_ڈیٹا(ثابت حرف *name)
{
    /*special case when no Table has yet been created we return nullptr */
    اگر (SHARED_ڈیٹا_HASHTABLE == خالی_اشاریہ)
    {
        واپس خالی_اشاریہ;
    }
    ورنہ
    {
        UConverter_Shared_ڈیٹا *rc;

        rc = static_cast<UConverter_Shared_ڈیٹا*>(uhash_get(SHARED_ڈیٹا_HASHTABLE, name));
        UCNV_DEBUG_LOG("get",name,rc);
        واپس rc;
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
ساکن UBool
ucnv_delete_Shared_Converter_ڈیٹا(UConverter_Shared_ڈیٹا * dead_Shared_ڈیٹا)
{
    UTRACE_اندراج_OC(UTRACE_UCNV_UNLOAD);
    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "unload converter %s shared data %p", dead_Shared_ڈیٹا->static_ڈیٹا->name, dead_Shared_ڈیٹا);

    اگر (dead_Shared_ڈیٹا->reference_شمار > 0) {
        UTRACE_EXIT_قدر((int32_t)جھوٹ);
        واپس جھوٹ;
    }

    اگر (dead_Shared_ڈیٹا->impl->unload != خالی_اشاریہ) {
        dead_Shared_ڈیٹا->impl->unload(dead_Shared_ڈیٹا);
    }

    اگر(dead_Shared_ڈیٹا->ڈیٹا_حافظہ != خالی_اشاریہ)
    {
        UData_حافظہ *ڈیٹا = (UData_حافظہ*)dead_Shared_ڈیٹا->ڈیٹا_حافظہ;
        udata_close(ڈیٹا);
    }

    uprv_free(dead_Shared_ڈیٹا);

    UTRACE_EXIT_قدر((int32_t)سچ);
    واپس سچ;
}

/**
 * Load a non-algorithmic converter.
 * If pkg==nullptr, then this function must be called inside umtx_lock(&cnvCacheMutex).
 */
UConverter_Shared_ڈیٹا *
ucnv_بھرو(UConverter_بھرو_دلائل *p_دلائل, UError_رمز *err) {
    UConverter_Shared_ڈیٹا *my_Shared_Converter_ڈیٹا;

    اگر(err == خالی_اشاریہ || U_FAILURE(*err)) {
        واپس خالی_اشاریہ;
    }

    اگر(p_دلائل->pkg != خالی_اشاریہ && *p_دلائل->pkg != 0) {
        /* application-provided converters are not currently cached */
        واپس createConverterFromFile(p_دلائل, err);
    }

    my_Shared_Converter_ڈیٹا = ucnv_get_Shared_Converter_ڈیٹا(p_دلائل->name);
    اگر (my_Shared_Converter_ڈیٹا == خالی_اشاریہ)
    {
        /*Not cached, we need to stream it in from file */
        my_Shared_Converter_ڈیٹا = createConverterFromFile(p_دلائل, err);
        اگر (U_FAILURE (*err) || (my_Shared_Converter_ڈیٹا == خالی_اشاریہ))
        {
            واپس خالی_اشاریہ;
        }
        ورنہ اگر (!p_دلائل->onlyTestIsLoadable)
        {
            /* share it with other library clients */
            ucnv_share_Converter_ڈیٹا(my_Shared_Converter_ڈیٹا);
        }
    }
    ورنہ
    {
        /* The data for this converter was already in the cache.            */
        /* Update the reference counter on the shared data: one more client */
        my_Shared_Converter_ڈیٹا->reference_شمار++;
    }

    واپس my_Shared_Converter_ڈیٹا;
}

/**
 * Unload a non-algorithmic converter.
 * It must be sharedData->isReferenceCounted
 * and this function must be called inside umtx_lock(&cnvCacheMutex).
 */
U_CAPI خالی
ucnv_unload(UConverter_Shared_ڈیٹا *shared_ڈیٹا) {
    اگر(shared_ڈیٹا != خالی_اشاریہ) {
        اگر (shared_ڈیٹا->reference_شمار > 0) {
            shared_ڈیٹا->reference_شمار--;
        }

        اگر((shared_ڈیٹا->reference_شمار <= 0)&&(shared_ڈیٹا->shared_ڈیٹا_Cached == جھوٹ)) {
            ucnv_delete_Shared_Converter_ڈیٹا(shared_ڈیٹا);
        }
    }
}

U_CFUNC خالی
ucnv_unload_Shared_ڈیٹا_If_Ready(UConverter_Shared_ڈیٹا *shared_ڈیٹا)
{
    اگر(shared_ڈیٹا != خالی_اشاریہ && shared_ڈیٹا->isReferenceCounted) {
        umtx_تالا(&cnvCacheMutex);
        ucnv_unload(shared_ڈیٹا);
        umtx_unlock(&cnvCacheMutex);
    }
}

U_CFUNC خالی
ucnv_increment_Ref_گنتی(UConverter_Shared_ڈیٹا *shared_ڈیٹا)
{
    اگر(shared_ڈیٹا != خالی_اشاریہ && shared_ڈیٹا->isReferenceCounted) {
        umtx_تالا(&cnvCacheMutex);
        shared_ڈیٹا->reference_شمار++;
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
ساکن خالی
parseConverterOptions(ثابت حرف *اندر_Name,
                      UConverterNamePieces *pPieces,
                      UConverter_بھرو_دلائل *p_دلائل,
                      UError_رمز *err)
{
    حرف *cnvName = pPieces->cnvName;
    حرف c;
    int32_t len = 0;

    p_دلائل->name=اندر_Name;
    p_دلائل->locale=pPieces->locale;
    p_دلائل->options=pPieces->options;

    /* copy the converter name itself to cnvName */
    جب((c=*اندر_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
        اگر (++len>=UCNV_MAX_CONVERTER_NAME_لمبائی) {
            *err = U_ILLEGAL_دلیل_ERROR;    /* bad name */
            pPieces->cnvName[0]=0;
            واپس;
        }
        *cnvName++=c;
        اندر_Name++;
    }
    *cnvName=0;
    p_دلائل->name=pPieces->cnvName;

    /* parse options. No more name copying should occur. */
    جب((c=*اندر_Name)!=0) {
        اگر(c==UCNV_OPTION_SEP_CHAR) {
            ++اندر_Name;
        }

        /* inName is behind an option separator */
        اگر(uprv_strncmp(اندر_Name, "locale=", 7)==0) {
            /* do not modify locale itself in case we have multiple locale options */
            حرف *dest=pPieces->locale;

            /* copy the locale option value */
            اندر_Name+=7;
            len=0;
            جب((c=*اندر_Name)!=0 && c!=UCNV_OPTION_SEP_CHAR) {
                ++اندر_Name;

                اگر(++len>=ULOC_FULLNAME_استعداد) {
                    *err=U_ILLEGAL_دلیل_ERROR;    /* bad name */
                    pPieces->locale[0]=0;
                    واپس;
                }

                *dest++=c;
            }
            *dest=0;
        } ورنہ اگر(uprv_strncmp(اندر_Name, "version=", 8)==0) {
            /* copy the version option value into bits 3..0 of pPieces->options */
            اندر_Name+=8;
            c=*اندر_Name;
            اگر(c==0) {
                p_دلائل->options=(pPieces->options&=~UCNV_OPTION_نسخہ);
                واپس;
            } ورنہ اگر (static_cast<uint8_t>(c - '0') < 10) {
                p_دلائل->options = pPieces->options = (pPieces->options & ~UCNV_OPTION_نسخہ) | static_cast<uint32_t>(c - '0');
                ++اندر_Name;
            }
        } ورنہ اگر(uprv_strncmp(اندر_Name, "swaplfnl", 8)==0) {
            اندر_Name+=8;
            p_دلائل->options=(pPieces->options|=UCNV_OPTION_تبادلہ_LFNL);
        /* add processing for new options here with another } else if(uprv_strncmp(inName, "option-name=", XX)==0) { */
        } ورنہ {
            /* ignore any other options until we define some */
            جب(((c = *اندر_Name++) != 0) && (c != UCNV_OPTION_SEP_CHAR)) {
            }
            اگر(c==0) {
                واپس;
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
U_CFUNC UConverter_Shared_ڈیٹا *
ucnv_بھرو_Shared_ڈیٹا(ثابت حرف *converterName,
                    UConverterNamePieces *pPieces,
                    UConverter_بھرو_دلائل *p_دلائل,
                    UError_رمز * err) {
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_بھرو_دلائل ڈھیر_دلائل;
    UConverter_Shared_ڈیٹا *my_Shared_Converter_ڈیٹا = خالی_اشاریہ;
    UError_رمز اندرونی_Error_رمز = U_ZERO_ERROR;
    UBool mayContainOption = سچ;
    UBool checkForAlgorithmic = سچ;

    اگر (U_FAILURE (*err)) {
        واپس خالی_اشاریہ;
    }

    اگر(pPieces == خالی_اشاریہ) {
        اگر(p_دلائل != خالی_اشاریہ) {
            /*
             * Bad: We may set pArgs pointers to stackPieces fields
             * which will be invalid after this function returns.
             */
            *err = U_اندرونی_پروگرام_ERROR;
            واپس خالی_اشاریہ;
        }
        pPieces = &ڈھیر_Pieces;
    }
    اگر(p_دلائل == خالی_اشاریہ) {
        uprv_memset(&ڈھیر_دلائل, 0, حجم(ڈھیر_دلائل));
        ڈھیر_دلائل.size = (int32_t)حجم(ڈھیر_دلائل);
        p_دلائل = &ڈھیر_دلائل;
    }

    pPieces->cnvName[0] = 0;
    pPieces->locale[0] = 0;
    pPieces->options = 0;

    p_دلائل->name = converterName;
    p_دلائل->locale = pPieces->locale;
    p_دلائل->options = pPieces->options;

    /* In case "name" is nullptr we want to open the default converter. */
    اگر (converterName == خالی_اشاریہ) {
#اگر U_CHARSET_IS_UTF8
        p_دلائل->name = "UTF-8";
        واپس (UConverter_Shared_ڈیٹا *)converter_ڈیٹا[UCNV_UTF8];
#ورنہ
        /* Call ucnv_getDefaultName first to query the name from the OS. */
        p_دلائل->name = ucnv_getDefaultName();
        اگر (p_دلائل->name == خالی_اشاریہ) {
            *err = U_MISSING_RESOURCE_ERROR;
            واپس خالی_اشاریہ;
        }
        my_Shared_Converter_ڈیٹا = (UConverter_Shared_ڈیٹا *)g_Default_Algorithmic_Shared_ڈیٹا;
        checkForAlgorithmic = جھوٹ;
        mayContainOption = g_Default_Converter_شامل_ہے_Option;
        /* the default converter name is already canonical */
#اگر_ختم
    }
    ورنہ اگر(UCNV_FAST_IS_UTF8(converterName)) {
        /* fastpath for UTF-8 */
        p_دلائل->name = "UTF-8";
        واپس (UConverter_Shared_ڈیٹا *)converter_ڈیٹا[UCNV_UTF8];
    }
    ورنہ {
        /* separate the converter name from the options */
        parseConverterOptions(converterName, pPieces, p_دلائل, err);
        اگر (U_FAILURE(*err)) {
            /* Very bad name used. */
            واپس خالی_اشاریہ;
        }

        /* get the canonical converter name */
        p_دلائل->name = ucnv_io_getConverterName(p_دلائل->name, &mayContainOption, &اندرونی_Error_رمز);
        اگر (U_FAILURE(اندرونی_Error_رمز) || p_دلائل->name == خالی_اشاریہ) {
            /*
            * set the input name in case the converter was added
            * without updating the alias table, or when there is no alias table
            */
            p_دلائل->name = pPieces->cnvName;
        } ورنہ اگر (اندرونی_Error_رمز == U_AMBIGUOUS_ALIAS_WARNING) {
            *err = U_AMBIGUOUS_ALIAS_WARNING;
        }
    }

    /* separate the converter name from the options */
    اگر(mayContainOption && p_دلائل->name != pPieces->cnvName) {
        parseConverterOptions(p_دلائل->name, pPieces, p_دلائل, err);
    }

    /* get the shared data for an algorithmic converter, if it is one */
    اگر (checkForAlgorithmic) {
        my_Shared_Converter_ڈیٹا = (UConverter_Shared_ڈیٹا *)getAlgorithmicTypeFromName(p_دلائل->name);
    }
    اگر (my_Shared_Converter_ڈیٹا == خالی_اشاریہ)
    {
        /* it is a data-based converter, get its shared data.               */
        /* Hold the cnvCacheMutex through the whole process of checking the */
        /*   converter data cache, and adding new entries to the cache      */
        /*   to prevent other threads from modifying the cache during the   */
        /*   process.                                                       */
        p_دلائل->nestedLoads=1;
        p_دلائل->pkg=خالی_اشاریہ;

        umtx_تالا(&cnvCacheMutex);
        my_Shared_Converter_ڈیٹا = ucnv_بھرو(p_دلائل, err);
        umtx_unlock(&cnvCacheMutex);
        اگر (U_FAILURE (*err) || (my_Shared_Converter_ڈیٹا == خالی_اشاریہ))
        {
            واپس خالی_اشاریہ;
        }
    }

    واپس my_Shared_Converter_ڈیٹا;
}

U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ثابت حرف* converterName,
                     UError_رمز* err)
{
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_بھرو_دلائل ڈھیر_دلائل=UCNV_بھرو_دلائل_INITIALIZER;
    UConverter_Shared_ڈیٹا *my_Shared_Converter_ڈیٹا;

    UTRACE_اندراج_OC(UTRACE_UCNV_OPEN);

    اگر(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open converter %s", converterName);

        my_Shared_Converter_ڈیٹا = ucnv_بھرو_Shared_ڈیٹا(converterName, &ڈھیر_Pieces, &ڈھیر_دلائل, err);

        myUConverter = ucnv_create_Converter_From_Shared_ڈیٹا(
            myUConverter, my_Shared_Converter_ڈیٹا,
            &ڈھیر_دلائل,
            err);

        اگر(U_SUCCESS(*err)) {
            UTRACE_EXIT_PTR_حالت(myUConverter, *err);
            واپس myUConverter;
        }
    }

    /* exit with error */
    UTRACE_EXIT_حالت(*err);
    واپس خالی_اشاریہ;
}

U_CFUNC UBool
ucnv_canCreateConverter(ثابت حرف *converterName, UError_رمز *err) {
    UConverter myUConverter;
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_بھرو_دلائل ڈھیر_دلائل=UCNV_بھرو_دلائل_INITIALIZER;
    UConverter_Shared_ڈیٹا *my_Shared_Converter_ڈیٹا;

    UTRACE_اندراج_OC(UTRACE_UCNV_OPEN);

    اگر(U_SUCCESS(*err)) {
        UTRACE_DATA1(UTRACE_OPEN_CLOSE, "test if can open converter %s", converterName);

        ڈھیر_دلائل.onlyTestIsLoadable=سچ;
        my_Shared_Converter_ڈیٹا = ucnv_بھرو_Shared_ڈیٹا(converterName, &ڈھیر_Pieces, &ڈھیر_دلائل, err);
        ucnv_create_Converter_From_Shared_ڈیٹا(
            &myUConverter, my_Shared_Converter_ڈیٹا,
            &ڈھیر_دلائل,
            err);
        ucnv_unload_Shared_ڈیٹا_If_Ready(my_Shared_Converter_ڈیٹا);
    }

    UTRACE_EXIT_حالت(*err);
    واپس U_SUCCESS(*err);
}

UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ثابت حرف* locale,
                                uint32_t options,
                                UError_رمز* err) {
    UConverter *cnv;
    ثابت UConverter_Shared_ڈیٹا *shared_ڈیٹا;
    UConverter_بھرو_دلائل ڈھیر_دلائل=UCNV_بھرو_دلائل_INITIALIZER;

    UTRACE_اندراج_OC(UTRACE_UCNV_OPEN_ALGORITHMIC);
    UTRACE_DATA1(UTRACE_OPEN_CLOSE, "open algorithmic converter type %d", (int32_t)type);

    اگر(type<0 || UCNV_نمبر_کا_SUPPORTED_CONVERTER_TYPES<=type) {
        *err = U_ILLEGAL_دلیل_ERROR;
        UTRACE_EXIT_حالت(U_ILLEGAL_دلیل_ERROR);
        واپس خالی_اشاریہ;
    }

    shared_ڈیٹا = converter_ڈیٹا[type];
    اگر(shared_ڈیٹا == خالی_اشاریہ || shared_ڈیٹا->isReferenceCounted) {
        /* not a valid type, or not an algorithmic converter */
        *err = U_ILLEGAL_دلیل_ERROR;
        UTRACE_EXIT_حالت(U_ILLEGAL_دلیل_ERROR);
        واپس خالی_اشاریہ;
    }

    ڈھیر_دلائل.name = "";
    ڈھیر_دلائل.options = options;
    ڈھیر_دلائل.locale=locale;
    cnv = ucnv_create_Converter_From_Shared_ڈیٹا(
            myUConverter, const_cast<UConverter_Shared_ڈیٹا*>(shared_ڈیٹا),
            &ڈھیر_دلائل, err);

    UTRACE_EXIT_PTR_حالت(cnv, *err);
    واپس cnv;
}

U_CFUNC UConverter*
ucnv_createConverterFromPackage(ثابت حرف *packageName, ثابت حرف *converterName, UError_رمز * err)
{
    UConverter *myUConverter;
    UConverter_Shared_ڈیٹا *my_Shared_Converter_ڈیٹا;
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_بھرو_دلائل ڈھیر_دلائل=UCNV_بھرو_دلائل_INITIALIZER;

    UTRACE_اندراج_OC(UTRACE_UCNV_OPEN_PACKAGE);

    اگر(U_FAILURE(*err)) {
        UTRACE_EXIT_حالت(*err);
        واپس خالی_اشاریہ;
    }

    UTRACE_DATA2(UTRACE_OPEN_CLOSE, "open converter %s from package %s", converterName, packageName);

    /* first, get the options out of the converterName string */
    ڈھیر_Pieces.cnvName[0] = 0;
    ڈھیر_Pieces.locale[0] = 0;
    ڈھیر_Pieces.options = 0;
    parseConverterOptions(converterName, &ڈھیر_Pieces, &ڈھیر_دلائل, err);
    اگر (U_FAILURE(*err)) {
        /* Very bad name used. */
        UTRACE_EXIT_حالت(*err);
        واپس خالی_اشاریہ;
    }
    ڈھیر_دلائل.nestedLoads=1;
    ڈھیر_دلائل.pkg=packageName;

    /* open the data, unflatten the shared structure */
    my_Shared_Converter_ڈیٹا = createConverterFromFile(&ڈھیر_دلائل, err);

    اگر (U_FAILURE(*err)) {
        UTRACE_EXIT_حالت(*err);
        واپس خالی_اشاریہ;
    }

    /* create the actual converter */
    myUConverter = ucnv_create_Converter_From_Shared_ڈیٹا(خالی_اشاریہ, my_Shared_Converter_ڈیٹا, &ڈھیر_دلائل, err);

    اگر (U_FAILURE(*err)) {
        ucnv_close(myUConverter);
        UTRACE_EXIT_حالت(*err);
        واپس خالی_اشاریہ;
    }

    UTRACE_EXIT_PTR_حالت(myUConverter, *err);
    واپس myUConverter;
}


U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_ڈیٹا(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_ڈیٹا* my_Shared_Converter_ڈیٹا,
                                   UConverter_بھرو_دلائل* p_دلائل,
                                   UError_رمز* err)
{
    UBool isCopyLocal;

    اگر(U_FAILURE(*err)) {
        ucnv_unload_Shared_ڈیٹا_If_Ready(my_Shared_Converter_ڈیٹا);
        واپس myUConverter;
    }
    اگر(myUConverter == خالی_اشاریہ)
    {
        myUConverter = (UConverter *) uprv_malloc (حجم (UConverter));
        اگر(myUConverter == خالی_اشاریہ)
        {
            *err = U_حافظہ_تخصیص_ERROR;
            ucnv_unload_Shared_ڈیٹا_If_Ready(my_Shared_Converter_ڈیٹا);
            واپس خالی_اشاریہ;
        }
        isCopyLocal = جھوٹ;
    } ورنہ {
        isCopyLocal = سچ;
    }

    /* initialize the converter */
    uprv_memset(myUConverter, 0, حجم(UConverter));
    myUConverter->isCopyLocal = isCopyLocal;
    /*myUConverter->isExtraLocal = false;*/ /* Set by the memset call */
    myUConverter->shared_ڈیٹا = my_Shared_Converter_ڈیٹا;
    myUConverter->options = p_دلائل->options;
    اگر(!p_دلائل->onlyTestIsLoadable) {
        myUConverter->preFromUFirstCP = U_SENTINEL;
        myUConverter->fromCharErrorBehaviour = UCNV_مقصد_U_DEFAULT_CALLBACK;
        myUConverter->fromUCharErrorBehaviour = UCNV_FROM_U_DEFAULT_CALLBACK;
        myUConverter->مقصد_Unicode_حالت = my_Shared_Converter_ڈیٹا->مقصد_Unicode_حالت;
        myUConverter->max_بائٹیں_فی_UChar = my_Shared_Converter_ڈیٹا->static_ڈیٹا->max_بائٹیں_فی_Char;
        myUConverter->subChar1 = my_Shared_Converter_ڈیٹا->static_ڈیٹا->subChar1;
        myUConverter->subCharLen = my_Shared_Converter_ڈیٹا->static_ڈیٹا->subCharLen;
        myUConverter->subChars = (uint8_t *)myUConverter->subUChars;
        uprv_memcpy(myUConverter->subChars, my_Shared_Converter_ڈیٹا->static_ڈیٹا->subChar, myUConverter->subCharLen);
        myUConverter->مقصد_UCallback_Reason = UCNV_ILLEGAL; /* default reason to invoke (*fromCharErrorBehaviour) */
    }

    اگر(my_Shared_Converter_ڈیٹا->impl->open != خالی_اشاریہ) {
        my_Shared_Converter_ڈیٹا->impl->open(myUConverter, p_دلائل, err);
        اگر(U_FAILURE(*err) && !p_دلائل->onlyTestIsLoadable) {
            /* don't ucnv_close() if onlyTestIsLoadable because not fully initialized */
            ucnv_close(myUConverter);
            واپس خالی_اشاریہ;
        }
    }

    واپس myUConverter;
}

/*Frees all shared immutable objects that aren't referred to (reference count = 0)
 */
U_CAPI int32_t U_EXPORT2
ucnv_flushCache ()
{
    UConverter_Shared_ڈیٹا *my_Shared_ڈیٹا = خالی_اشاریہ;
    int32_t pos;
    int32_t جدول_Deleted_عدد = 0;
    ثابت UHashElement *e;
    /*UErrorCode status = U_ILLEGAL_ARGUMENT_ERROR;*/
    int32_t i, بقایا;

    UTRACE_اندراج_OC(UTRACE_UCNV_FLUSH_CACHE);

    /* Close the default converter without creating a new one so that everything will be flushed. */
    u_flushDefaultConverter();

    /*if shared data hasn't even been lazy evaluated yet
    * return 0
    */
    اگر (SHARED_ڈیٹا_HASHTABLE == خالی_اشاریہ) {
        UTRACE_EXIT_قدر((int32_t)0);
        واپس 0;
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
    umtx_تالا(&cnvCacheMutex);
    /*
     * double loop: A delta/extension-only converter has a pointer to its base table's
     * shared data; the first iteration of the outer loop may see the delta converter
     * before the base converter, and unloading the delta converter may get the base
     * converter's reference counter down to 0.
     */
    i = 0;
    کرو {
        بقایا = 0;
        pos = UHASH_پہلا;
        جب ((e = uhash_اگلا_Element (SHARED_ڈیٹا_HASHTABLE, &pos)) != خالی_اشاریہ)
        {
            my_Shared_ڈیٹا = (UConverter_Shared_ڈیٹا *) e->value.اشارہ_گر;
            /*deletes only if reference counter == 0 */
            اگر (my_Shared_ڈیٹا->reference_شمار == 0)
            {
                جدول_Deleted_عدد++;

                UCNV_DEBUG_LOG("del",my_Shared_ڈیٹا->static_ڈیٹا->name,my_Shared_ڈیٹا);

                uhash_removeElement(SHARED_ڈیٹا_HASHTABLE, e);
                my_Shared_ڈیٹا->shared_ڈیٹا_Cached = جھوٹ;
                ucnv_delete_Shared_Converter_ڈیٹا (my_Shared_ڈیٹا);
            } ورنہ {
                ++بقایا;
            }
        }
    } جب(++i == 1 && بقایا > 0);
    umtx_unlock(&cnvCacheMutex);

    UTRACE_DATA1(UTRACE_INFO, "ucnv_flushCache() exits with %d converters remaining", بقایا);

    UTRACE_EXIT_قدر(جدول_Deleted_عدد);
    واپس جدول_Deleted_عدد;
}

/* available converters list --------------------------------------------------- */

ساکن خالی U_CALLCONV init_دستیاب_Converters_List(UError_رمز &err_رمز) {
    U_ASSERT(g_دستیاب_Converter_گنتی == 0);
    U_ASSERT(g_دستیاب_Converters == خالی_اشاریہ);

    ucnv_enableCleanup();
    UEnumeration *allConvEnum = ucnv_open_All_نام(&err_رمز);
    int32_t all_Converter_گنتی = uenum_گنتی(allConvEnum, &err_رمز);
    اگر (U_FAILURE(err_رمز)) {
        واپس;
    }

    /* We can't have more than "*converterTable" converters to open */
    g_دستیاب_Converters = static_cast<ثابت حرف**>(uprv_malloc(all_Converter_گنتی * حجم(حرف*)));
    اگر (!g_دستیاب_Converters) {
        err_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    /* Open the default converter to make sure that it has first dibs in the hash table. */
    UError_رمز local_حالت = U_ZERO_ERROR;
    UConverter عارضی_Converter;
    ucnv_close(ucnv_createConverter(&عارضی_Converter, خالی_اشاریہ, &local_حالت));

    g_دستیاب_Converter_گنتی = 0;

    برائے (int32_t idx = 0; idx < all_Converter_گنتی; idx++) {
        local_حالت = U_ZERO_ERROR;
        ثابت حرف *converterName = uenum_اگلا(allConvEnum, خالی_اشاریہ, &local_حالت);
        اگر (ucnv_canCreateConverter(converterName, &local_حالت)) {
            g_دستیاب_Converters[g_دستیاب_Converter_گنتی++] = converterName;
        }
    }

    uenum_close(allConvEnum);
}


ساکن UBool have_دستیاب_Converter_List(UError_رمز *p_Error_رمز) {
    umtx_initOnce(g_دستیاب_Converters_Init_Once, &init_دستیاب_Converters_List, *p_Error_رمز);
    واپس U_SUCCESS(*p_Error_رمز);
}

U_CFUNC uint16_t
ucnv_bld_گنتی_دستیاب_Converters(UError_رمز *p_Error_رمز) {
    اگر (have_دستیاب_Converter_List(p_Error_رمز)) {
        واپس g_دستیاب_Converter_گنتی;
    }
    واپس 0;
}

U_CFUNC ثابت حرف *
ucnv_bld_get_دستیاب_Converter(uint16_t n, UError_رمز *p_Error_رمز) {
    اگر (have_دستیاب_Converter_List(p_Error_رمز)) {
        اگر (n < g_دستیاب_Converter_گنتی) {
            واپس g_دستیاب_Converters[n];
        }
        *p_Error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
    }
    واپس خالی_اشاریہ;
}

/* default converter name --------------------------------------------------- */

#اگر !U_CHARSET_IS_UTF8
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
ساکن اندرونی خالی
اندرونی_سیٹ_Name(ثابت حرف *name, UError_رمز *حالت_2) {
    UConverterNamePieces ڈھیر_Pieces;
    UConverter_بھرو_دلائل ڈھیر_دلائل=UCNV_بھرو_دلائل_INITIALIZER;
    int32_t length=(int32_t)(uprv_strlen(name));
    UBool شامل_ہے_Option = (UBool)(uprv_strchr(name, UCNV_OPTION_SEP_CHAR) != خالی_اشاریہ);
    ثابت UConverter_Shared_ڈیٹا *algorithmic_Shared_ڈیٹا;

    ڈھیر_دلائل.name = name;
    اگر(شامل_ہے_Option) {
        ڈھیر_Pieces.cnvName[0] = 0;
        ڈھیر_Pieces.locale[0] = 0;
        ڈھیر_Pieces.options = 0;
        parseConverterOptions(name, &ڈھیر_Pieces, &ڈھیر_دلائل, حالت_2);
        اگر(U_FAILURE(*حالت_2)) {
            واپس;
        }
    }
    algorithmic_Shared_ڈیٹا = getAlgorithmicTypeFromName(ڈھیر_دلائل.name);

    umtx_تالا(&cnvCacheMutex);

    g_Default_Algorithmic_Shared_ڈیٹا = algorithmic_Shared_ڈیٹا;
    g_Default_Converter_شامل_ہے_Option = شامل_ہے_Option;
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
#اگر_ختم

/*
 * In order to be really thread-safe, the get function would have to take
 * a buffer parameter and copy the current string inside a mutex block.
 * This implementation only tries to be really thread-safe while
 * setting the name.
 * It assumes that setting a pointer is atomic.
 */

U_CAPI ثابت حرف*  U_EXPORT2
ucnv_getDefaultName() {
#اگر U_CHARSET_IS_UTF8
    واپس "UTF-8";
#ورنہ
    /* local variable to be thread-safe */
    ثابت حرف *name;

    /*
    Concurrent calls to ucnv_getDefaultName must be thread safe,
    but ucnv_setDefaultName is not thread safe.
    */
    {
        icu::Mutex تالا(&cnvCacheMutex);
        name = gDefaultConverterName;
    }
    اگر(name==خالی_اشاریہ) {
        UError_رمز error_رمز = U_ZERO_ERROR;
        UConverter *cnv = خالی_اشاریہ;

        name = uprv_getDefaultCodepage();

        /* if the name is there, test it out and get the canonical name with options */
        اگر(name != خالی_اشاریہ) {
            cnv = ucnv_open(name, &error_رمز);
            اگر(U_SUCCESS(error_رمز) && cnv != خالی_اشاریہ) {
                name = ucnv_getName(cnv, &error_رمز);
            }
        }

        اگر(name == خالی_اشاریہ || name[0] == 0
            || U_FAILURE(error_رمز) || cnv == خالی_اشاریہ
            || uprv_strlen(name)>=حجم(g_Default_Converter_Name_بفر))
        {
            /* Panic time, let's use a fallback. */
#اگر (U_CHARSET_FAMILY == U_ASCII_FAMILY)
            name = "US-ASCII";
            /* there is no 'algorithmic' converter for EBCDIC */
#وگرنہ_اگر U_PLATFORM == U_PF_OS390
            name = "ibm-1047_P100-1995" UCNV_تبادلہ_LFNL_OPTION_STRING;
#ورنہ
            name = "ibm-37_P100-1995";
#اگر_ختم
        }

        اندرونی_سیٹ_Name(name, &error_رمز);

        /* The close may make the current name go away. */
        ucnv_close(cnv);
    }

    واپس name;
#اگر_ختم
}

#اگر U_CHARSET_IS_UTF8
U_CAPI خالی U_EXPORT2 ucnv_سیٹ_Default_Name(ثابت حرف *) {}
#ورنہ
/*
This function is not thread safe, and it can't be thread safe.
See internalSetName or the API reference for details.
*/
U_CAPI خالی U_EXPORT2
ucnv_سیٹ_Default_Name(ثابت حرف *converterName) {
    اگر(converterName==خالی_اشاریہ) {
        /* reset to the default codepage */
        gDefaultConverterName=خالی_اشاریہ;
    } ورنہ {
        UError_رمز error_رمز = U_ZERO_ERROR;
        UConverter *cnv = خالی_اشاریہ;
        ثابت حرف *name = خالی_اشاریہ;

        /* if the name is there, test it out and get the canonical name with options */
        cnv = ucnv_open(converterName, &error_رمز);
        اگر(U_SUCCESS(error_رمز) && cnv != خالی_اشاریہ) {
            name = ucnv_getName(cnv, &error_رمز);
        }

        اگر(U_SUCCESS(error_رمز) && name!=خالی_اشاریہ) {
            اندرونی_سیٹ_Name(name, &error_رمز);
        }
        /* else this converter is bad to use. Don't change it to a bad value. */

        /* The close may make the current name go away. */
        ucnv_close(cnv);

        /* reset the converter cache */
        u_flushDefaultConverter();
    }
}
#اگر_ختم

/* data swapping ------------------------------------------------------------ */

/* most of this might belong more properly into ucnvmbcs.c, but that is so large */

#اگر !UCONFIG_NO_LEGACY_CONVERSION

U_CAPI int32_t U_EXPORT2
ucnv_تبادلہ(ثابت UDataSwapper *ds,
          ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
          UError_رمز *p_Error_رمز) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_سائز;

    ثابت uint8_t *اندر_بائٹیں;
    uint8_t *برآمد_بائٹیں;

    uint32_t انحراف, count, static_ڈیٹا_سائز;
    int32_t size;

    ثابت UConverter_Static_ڈیٹا *اندر_Static_ڈیٹا;
    UConverter_Static_ڈیٹا *برآمد_Static_ڈیٹا;

    ثابت _MBCSHeader *اندر_MBCSHeader;
    _MBCSHeader *برآمد_MBCSHeader;
    _MBCSHeader mbcs_سرخی;
    uint32_t mbcs_سرخی_لمبائی;
    UBool noFromU=جھوٹ;

    uint8_t outputType;

    int32_t maxFastUChar, mbcs_اشاریہ_لمبائی;

    ثابت int32_t *اندر_Ext_Indexes;
    int32_t ext_انحراف;

    /* udata_swapDataHeader checks the arguments */
    سرخی_سائز=udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
    اگر(!(
        pInfo->ڈیٹا_Format[0]==0x63 &&   /* dataFormat="cnvt" */
        pInfo->ڈیٹا_Format[1]==0x6e &&
        pInfo->ڈیٹا_Format[2]==0x76 &&
        pInfo->ڈیٹا_Format[3]==0x74 &&
        pInfo->format_نسخہ[0]==6 &&
        pInfo->format_نسخہ[1]>=2
    )) {
        udata_printError(ds, "ucnv_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not recognized as an ICU .cnv conversion table\n",
                         pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1],
                         pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3],
                         pInfo->format_نسخہ[0], pInfo->format_نسخہ[1]);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اندر_بائٹیں=(ثابت uint8_t *)اندر_ڈیٹا+سرخی_سائز;
    برآمد_بائٹیں=(برآمد_ڈیٹا == خالی_اشاریہ) ? خالی_اشاریہ : (uint8_t *)برآمد_ڈیٹا+سرخی_سائز;

    /* read the initial UConverterStaticData structure after the UDataInfo header */
    اندر_Static_ڈیٹا=(ثابت UConverter_Static_ڈیٹا *)اندر_بائٹیں;
    برآمد_Static_ڈیٹا=(UConverter_Static_ڈیٹا *)برآمد_بائٹیں;

    اگر(length<0) {
        static_ڈیٹا_سائز=ds->readUInt32(اندر_Static_ڈیٹا->struct_سائز);
    } ورنہ {
        length-=سرخی_سائز;
        اگر( length<(int32_t)حجم(UConverter_Static_ڈیٹا) ||
            (uint32_t)length<(static_ڈیٹا_سائز=ds->readUInt32(اندر_Static_ڈیٹا->struct_سائز))
        ) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after header) for an ICU .cnv conversion table\n",
                             length);
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
    }

    اگر(length>=0) {
        /* swap the static data */
        اگر(اندر_Static_ڈیٹا!=برآمد_Static_ڈیٹا) {
            uprv_memcpy(برآمد_Static_ڈیٹا, اندر_Static_ڈیٹا, static_ڈیٹا_سائز);
        }

        ds->تبادلہ_Array32(ds, &اندر_Static_ڈیٹا->struct_سائز, 4,
                           &برآمد_Static_ڈیٹا->struct_سائز, p_Error_رمز);
        ds->تبادلہ_Array32(ds, &اندر_Static_ڈیٹا->codepage, 4,
                           &برآمد_Static_ڈیٹا->codepage, p_Error_رمز);

        ds->تبادلہ_Inv_Chars(ds, اندر_Static_ڈیٹا->name, (int32_t)uprv_strlen(اندر_Static_ڈیٹا->name),
                            برآمد_Static_ڈیٹا->name, p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            udata_printError(ds, "ucnv_swap(): error swapping converter name\n");
            واپس 0;
        }
    }

    اندر_بائٹیں+=static_ڈیٹا_سائز;
    اگر (برآمد_بائٹیں != خالی_اشاریہ) برآمد_بائٹیں+=static_ڈیٹا_سائز;
    اگر(length>=0) {
        length-=(int32_t)static_ڈیٹا_سائز;
    }

    /* check for supported conversionType values */
    اگر(اندر_Static_ڈیٹا->conversionType==UCNV_MBCS) {
        /* swap MBCS data */
        اندر_MBCSHeader=(ثابت _MBCSHeader *)اندر_بائٹیں;
        برآمد_MBCSHeader=(_MBCSHeader *)برآمد_بائٹیں;

        اگر(0<=length && length<(int32_t)حجم(_MBCSHeader)) {
            udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                length);
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
        اگر(اندر_MBCSHeader->نسخہ[0]==4 && اندر_MBCSHeader->نسخہ[1]>=1) {
            mbcs_سرخی_لمبائی=MBCS_سرخی_V4_لمبائی;
        } ورنہ اگر(اندر_MBCSHeader->نسخہ[0]==5 && اندر_MBCSHeader->نسخہ[1]>=3 &&
                  ((mbcs_سرخی.options=ds->readUInt32(اندر_MBCSHeader->options))&
                   MBCS_OPT_UNKNOWN_INCOMPATIBLE_نقاب)==0
        ) {
            mbcs_سرخی_لمبائی=mbcs_سرخی.options&MBCS_OPT_لمبائی_نقاب;
            noFromU = (mbcs_سرخی.options & MBCS_OPT_NO_FROM_U) != 0;
        } ورنہ {
            udata_printError(ds, "ucnv_swap(): unsupported _MBCSHeader.version %d.%d\n",
                             اندر_MBCSHeader->نسخہ[0], اندر_MBCSHeader->نسخہ[1]);
            *p_Error_رمز=U_UNSUPPORTED_ERROR;
            واپس 0;
        }

        uprv_memcpy(mbcs_سرخی.نسخہ, اندر_MBCSHeader->نسخہ, 4);
        mbcs_سرخی.گنتی_حالتیں=         ds->readUInt32(اندر_MBCSHeader->گنتی_حالتیں);
        mbcs_سرخی.گنتی_مقصد_UFallbacks=   ds->readUInt32(اندر_MBCSHeader->گنتی_مقصد_UFallbacks);
        mbcs_سرخی.انحراف_مقصد_UCode_Units=  ds->readUInt32(اندر_MBCSHeader->انحراف_مقصد_UCode_Units);
        mbcs_سرخی.انحراف_From_UTable=    ds->readUInt32(اندر_MBCSHeader->انحراف_From_UTable);
        mbcs_سرخی.انحراف_From_UBytes=    ds->readUInt32(اندر_MBCSHeader->انحراف_From_UBytes);
        mbcs_سرخی.flags=               ds->readUInt32(اندر_MBCSHeader->flags);
        mbcs_سرخی.from_UBytes_لمبائی=    ds->readUInt32(اندر_MBCSHeader->from_UBytes_لمبائی);
        /* mbcsHeader.options have been read above */

        ext_انحراف=(int32_t)(mbcs_سرخی.flags>>8);
        outputType=(uint8_t)mbcs_سرخی.flags;
        اگر(noFromU && outputType==MBCS_OUTPUT_1) {
            udata_printError(ds, "ucnv_swap(): unsupported combination of makeconv --small with SBCS\n");
            *p_Error_رمز=U_UNSUPPORTED_ERROR;
            واپس 0;
        }

        /* make sure that the output type is known */
        انتخاب(outputType) {
        حالت MBCS_OUTPUT_1:
        حالت MBCS_OUTPUT_2:
        حالت MBCS_OUTPUT_3:
        حالت MBCS_OUTPUT_4:
        حالت MBCS_OUTPUT_3_EUC:
        حالت MBCS_OUTPUT_4_EUC:
        حالت MBCS_OUTPUT_2_SISO:
        حالت MBCS_OUTPUT_EXT_ONLY:
            /* OK */
            توڑو;
        طےشدہ:
            udata_printError(ds, "ucnv_swap(): unsupported MBCS output type 0x%x\n",
                             outputType);
            *p_Error_رمز=U_UNSUPPORTED_ERROR;
            واپس 0;
        }

        /* calculate the length of the MBCS data */

        /*
         * utf8Friendly MBCS files (mbcsHeader.version 4.3)
         * contain an additional mbcsIndex table:
         *   uint16_t[(maxFastUChar+1)>>6];
         * where maxFastUChar=((mbcsHeader.version[2]<<8)|0xff).
         */
        maxFastUChar=0;
        mbcs_اشاریہ_لمبائی=0;
        اگر( outputType!=MBCS_OUTPUT_EXT_ONLY && outputType!=MBCS_OUTPUT_1 &&
            mbcs_سرخی.نسخہ[1]>=3 && (maxFastUChar=mbcs_سرخی.نسخہ[2])!=0
        ) {
            maxFastUChar=(maxFastUChar<<8)|0xff;
            mbcs_اشاریہ_لمبائی=((maxFastUChar+1)>>6)*2;  /* number of bytes */
        }

        اگر(ext_انحراف==0) {
            size=(int32_t)(mbcs_سرخی.انحراف_From_UBytes+mbcs_اشاریہ_لمبائی);
            اگر(!noFromU) {
                size+=(int32_t)mbcs_سرخی.from_UBytes_لمبائی;
            }

            /* avoid compiler warnings - not otherwise necessary, and the value does not matter */
            اندر_Ext_Indexes=خالی_اشاریہ;
        } ورنہ {
            /* there is extension data after the base data, see ucnv_ext.h */
            اگر(length>=0 && length<(ext_انحراف+UCNV_EXT_INDEXES_MIN_لمبائی*4)) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table with extension data\n",
                                 length);
                *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                واپس 0;
            }

            اندر_Ext_Indexes=(ثابت int32_t *)(اندر_بائٹیں+ext_انحراف);
            size=ext_انحراف+udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_سائز]);
        }

        اگر(length>=0) {
            اگر(length<size) {
                udata_printError(ds, "ucnv_swap(): too few bytes (%d after headers) for an ICU MBCS .cnv conversion table\n",
                                 length);
                *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                واپس 0;
            }

            /* copy the data for inaccessible bytes */
            اگر(اندر_بائٹیں!=برآمد_بائٹیں) {
                uprv_memcpy(برآمد_بائٹیں, اندر_بائٹیں, size);
            }

            /* swap the MBCSHeader, except for the version field */
            count=mbcs_سرخی_لمبائی*4;
            ds->تبادلہ_Array32(ds, &اندر_MBCSHeader->گنتی_حالتیں, count-4,
                               &برآمد_MBCSHeader->گنتی_حالتیں, p_Error_رمز);

            اگر(outputType==MBCS_OUTPUT_EXT_ONLY) {
                /*
                 * extension-only file,
                 * contains a base name instead of normal base table data
                 */

                /* swap the base name, between the header and the extension data */
                ثابت حرف *اندر_بنیاد_Name=(ثابت حرف *)اندر_بائٹیں+count;
                حرف *برآمد_بنیاد_Name=(حرف *)برآمد_بائٹیں+count;
                ds->تبادلہ_Inv_Chars(ds, اندر_بنیاد_Name, (int32_t)uprv_strlen(اندر_بنیاد_Name),
                                    برآمد_بنیاد_Name, p_Error_رمز);
            } ورنہ {
                /* normal file with base table data */

                /* swap the state table, 1kB per state */
                انحراف=count;
                count=mbcs_سرخی.گنتی_حالتیں*1024;
                ds->تبادلہ_Array32(ds, اندر_بائٹیں+انحراف, (int32_t)count,
                                   برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* swap the toUFallbacks[] */
                انحراف+=count;
                count=mbcs_سرخی.گنتی_مقصد_UFallbacks*8;
                ds->تبادلہ_Array32(ds, اندر_بائٹیں+انحراف, (int32_t)count,
                                   برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* swap the unicodeCodeUnits[] */
                انحراف=mbcs_سرخی.انحراف_مقصد_UCode_Units;
                count=mbcs_سرخی.انحراف_From_UTable-انحراف;
                ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, (int32_t)count,
                                   برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* offset to the stage 1 table, independent of the outputType */
                انحراف=mbcs_سرخی.انحراف_From_UTable;

                اگر(outputType==MBCS_OUTPUT_1) {
                    /* SBCS: swap the fromU tables, all 16 bits wide */
                    count=(mbcs_سرخی.انحراف_From_UBytes-انحراف)+mbcs_سرخی.from_UBytes_لمبائی;
                    ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, (int32_t)count,
                                       برآمد_بائٹیں+انحراف, p_Error_رمز);
                } ورنہ {
                    /* otherwise: swap the stage tables separately */

                    /* stage 1 table: uint16_t[0x440 or 0x40] */
                    اگر(اندر_Static_ڈیٹا->unicode_نقاب&UCNV_HAS_SUPPLEMENTARY) {
                        count=0x440*2; /* for all of Unicode */
                    } ورنہ {
                        count=0x40*2; /* only BMP */
                    }
                    ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, (int32_t)count,
                                       برآمد_بائٹیں+انحراف, p_Error_رمز);

                    /* stage 2 table: uint32_t[] */
                    انحراف+=count;
                    count=mbcs_سرخی.انحراف_From_UBytes-انحراف;
                    ds->تبادلہ_Array32(ds, اندر_بائٹیں+انحراف, (int32_t)count,
                                       برآمد_بائٹیں+انحراف, p_Error_رمز);

                    /* stage 3/result bytes: sometimes uint16_t[] or uint32_t[] */
                    انحراف=mbcs_سرخی.انحراف_From_UBytes;
                    count= noFromU ? 0 : mbcs_سرخی.from_UBytes_لمبائی;
                    انتخاب(outputType) {
                    حالت MBCS_OUTPUT_2:
                    حالت MBCS_OUTPUT_3_EUC:
                    حالت MBCS_OUTPUT_2_SISO:
                        ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, (int32_t)count,
                                           برآمد_بائٹیں+انحراف, p_Error_رمز);
                        توڑو;
                    حالت MBCS_OUTPUT_4:
                        ds->تبادلہ_Array32(ds, اندر_بائٹیں+انحراف, (int32_t)count,
                                           برآمد_بائٹیں+انحراف, p_Error_رمز);
                        توڑو;
                    طےشدہ:
                        /* just uint8_t[], nothing to swap */
                        توڑو;
                    }

                    اگر(mbcs_اشاریہ_لمبائی!=0) {
                        انحراف+=count;
                        count=mbcs_اشاریہ_لمبائی;
                        ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, (int32_t)count,
                                           برآمد_بائٹیں+انحراف, p_Error_رمز);
                    }
                }
            }

            اگر(ext_انحراف!=0) {
                /* swap the extension data */
                اندر_بائٹیں+=ext_انحراف;
                برآمد_بائٹیں+=ext_انحراف;

                /* swap toUTable[] */
                انحراف=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_مقصد_U_اشاریہ]);
                length=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_مقصد_U_لمبائی]);
                ds->تبادلہ_Array32(ds, اندر_بائٹیں+انحراف, length*4, برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* swap toUUChars[] */
                انحراف=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_مقصد_U_UCHARS_اشاریہ]);
                length=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_مقصد_U_UCHARS_لمبائی]);
                ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, length*2, برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* swap fromUTableUChars[] */
                انحراف=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_FROM_U_UCHARS_اشاریہ]);
                length=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_FROM_U_لمبائی]);
                ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, length*2, برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* swap fromUTableValues[] */
                انحراف=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_FROM_U_اقدار_اشاریہ]);
                /* same length as for fromUTableUChars[] */
                ds->تبادلہ_Array32(ds, اندر_بائٹیں+انحراف, length*4, برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* no need to swap fromUBytes[] */

                /* swap fromUStage12[] */
                انحراف=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_اشاریہ]);
                length=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_12_لمبائی]);
                ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, length*2, برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* swap fromUStage3[] */
                انحراف=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_اشاریہ]);
                length=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_لمبائی]);
                ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, length*2, برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* swap fromUStage3b[] */
                انحراف=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_اشاریہ]);
                length=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_FROM_U_STAGE_3_B_لمبائی]);
                ds->تبادلہ_Array32(ds, اندر_بائٹیں+انحراف, length*4, برآمد_بائٹیں+انحراف, p_Error_رمز);

                /* swap indexes[] */
                length=udata_readInt32(ds, اندر_Ext_Indexes[UCNV_EXT_INDEXES_لمبائی]);
                ds->تبادلہ_Array32(ds, اندر_بائٹیں, length*4, برآمد_بائٹیں, p_Error_رمز);
            }
        }
    } ورنہ {
        udata_printError(ds, "ucnv_swap(): unknown conversionType=%d!=UCNV_MBCS\n",
                         اندر_Static_ڈیٹا->conversionType);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    واپس سرخی_سائز+(int32_t)static_ڈیٹا_سائز+size;
}

#اگر_ختم /* #if !UCONFIG_NO_LEGACY_CONVERSION */

#اگر_ختم
